/**
 * src/parser.c
 *
 * Implementation of the Recursive Descent Parser.
 */
#include "parser.h"
#include "utils.h" // For safe_strdup
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Forward Declarations of Static Helper Functions ---

/**
 * @brief Consumes the current token and gets the next one from the lexer.
 */
static void advance(Parser* parser);

/**
 * @brief Checks if the current token is of the expected type.
 * If it is, consumes it and returns true.
 * If not, sets an error and returns false.
 */
static int expect(Parser* parser, TokenType type, const char* error_msg);

/**
 * @brief Checks if the current token matches the given type.
 */
static int check(Parser* parser, TokenType type);

/**
 * @brief The core recursive function to parse a single element.
 * Handles <tag>, attributes, children, and </tag>.
 */
static DomNode* parse_element(Parser* parser);

/**
 * @brief Parses a list of attributes inside an opening tag.
 */
static void parse_attributes(Parser* parser, DomNode* node);

/**
 * @brief Parses a list of child nodes (either elements or text).
 * Stops when it finds a closing tag or EOF.
 */
static DomNode* parse_children(Parser* parser);

/**
 * @brief Parses a single node, which could be an element or text.
 */
static DomNode* parse_node(Parser* parser);

/**
 * @brief Checks if a tag is a known self-closing tag (e.g., "img", "br").
 */
static int is_self_closing_tag(const char* tag_name);

/**
 * @brief Sets the parser's error state.
 */
static void parser_error(Parser* parser, const char* message);

// --- Public Function Implementations ---

Parser* parser_init(Lexer* lexer) {
    Parser* parser = (Parser*)safe_malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->has_error = 0;
    parser->error_message = NULL;
    
    // Initialize with the first token
    parser->previous_token.lexeme = NULL;
    parser->current_token.lexeme = NULL;
    advance(parser);
    
    return parser;
}

void parser_free(Parser* parser) {
    if (parser) {
        free_token_lexeme(&parser->current_token);
        free_token_lexeme(&parser->previous_token);
        free(parser->error_message);
        // Note: The lexer is freed separately by the caller
        free(parser);
    }
}

DomNode* parse(Parser* parser) {
    // Create a dummy root node for the document
    DomNode* root = create_element_node("#document");
    
    // Parse all top-level nodes as children of the root
    root->first_child = parse_children(parser);

    if (parser->has_error) {
        free_dom_tree(root);
        return NULL;
    }

    return root;
}

// --- Static Helper Function Implementations ---

static void parser_error(Parser* parser, const char* message) {
    if (parser->has_error) return; // Only log the first error
    
    parser->has_error = 1;
    
    // Format a detailed error message
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "[Line %d, Col %d] Error: %s. (Got token %d: '%s')",
             parser->current_token.line,
             parser->current_token.col,
             message,
             parser->current_token.type,
             parser->current_token.lexeme);
             
    parser->error_message = safe_strdup(buffer);
}

static void advance(Parser* parser) {
    if (parser->has_error) return;

    // Free the *previous* token's lexeme
    free_token_lexeme(&parser->previous_token);

    // Move current to previous
    parser->previous_token = parser->current_token;

    // Get the next token from the lexer
    parser->current_token = get_next_token(parser->lexer);
    
    // Check for lexer errors
    if (parser->current_token.type == TOKEN_ERROR) {
        parser_error(parser, parser->current_token.lexeme);
    }
}

static int check(Parser* parser, TokenType type) {
    return parser->current_token.type == type;
}

static int expect(Parser* parser, TokenType type, const char* error_msg) {
    if (check(parser, type)) {
        advance(parser);
        return 1;
    }
    parser_error(parser, error_msg);
    return 0;
}

static DomNode* parse_node(Parser* parser) {
    if (check(parser, TOKEN_OPEN_TAG)) {
        return parse_element(parser);
    }
    if (check(parser, TOKEN_TEXT)) {
        DomNode* node = create_text_node(parser->current_token.lexeme);
        advance(parser); // Consume the text token
        return node;
    }
    
    // If it's not text or an element, it's an error (or EOF/CloseTag)
    return NULL;
}

static DomNode* parse_children(Parser* parser) {
    DomNode* first_child = NULL;
    DomNode* current_child = NULL;

    // Loop until we hit a closing tag or the end of the file
    while (!check(parser, TOKEN_CLOSE_TAG) && !check(parser, TOKEN_EOF)) {
        if (parser->has_error) return first_child;

        DomNode* child_node = parse_node(parser);
        if (child_node == NULL) {
            // This might happen if parse_node encounters an unexpected token
            // e.g. stray '>', '=', etc.
            // We can either error out or try to skip it.
            // Let's error out for robustness.
            if (!check(parser, TOKEN_EOF)) {
                 parser_error(parser, "Unexpected token while parsing children.");
            }
            break;
        }

        if (first_child == NULL) {
            first_child = child_node;
            current_child = child_node;
        } else {
            current_child->next_sibling = child_node;
            child_node->parent = current_child->parent; // Set same parent
            current_child = child_node;
        }
    }
    
    return first_child;
}

static void parse_attributes(Parser* parser, DomNode* node) {
    // Loop as long as we see attribute names
    while (check(parser, TOKEN_ATTR_NAME)) {
        if (parser->has_error) return;

        char* name = safe_strdup(parser->current_token.lexeme);
        advance(parser); // Consume attr name
        
        char* value = safe_strdup("true"); // Default for boolean attributes

        if (check(parser, TOKEN_ATTR_EQUALS)) {
            advance(parser); // Consume '='
            if (expect(parser, TOKEN_ATTR_VALUE, "Expected attribute value.")) {
                free(value); // Free the default "true"
                // The lexeme from ATTR_VALUE token has quotes, let's strip them
                // Our lexer is designed to strip them, but if not, do it here.
                // Our lexer *doesn't* include the quotes in the lexeme.
                value = safe_strdup(parser->previous_token.lexeme);
            }
        }
        
        add_attribute(node, name, value);
        free(name);
        free(value);
    }
}

static int is_self_closing_tag(const char* tag_name) {
    // List of common self-closing tags
    const char* self_closing[] = {
        "area", "base", "br", "col", "embed", "hr", "img", 
        "input", "link", "meta", "param", "source", "track", "wbr",
        NULL // Sentinel
    };
    
    for (int i = 0; self_closing[i]; i++) {
        if (strcmp(tag_name, self_closing[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static DomNode* parse_element(Parser* parser) {
    // Assumes the TOKEN_OPEN_TAG has just been checked
    DomNode* node = create_element_node(parser->current_token.lexeme);
    advance(parser); // Consume open tag token
    
    // 2. Parse attributes
    parse_attributes(parser, node);
    if (parser->has_error) return node; // Return partially built node

    // 3. Handle closing
    if (check(parser, TOKEN_SELF_CLOSE)) {
        // Case 1: <img />
        advance(parser); // Consume "/>"
        return node;
    }

    if (check(parser, TOKEN_GT)) {
        // Case 2: <p> ... </p> or <br>
        advance(parser); // Consume ">"
        
        // Check if it's a *known* self-closing tag that was written as <br>
        if (is_self_closing_tag(node->tag_name)) {
            return node;
        }

        // 4. Parse children
        node->first_child = parse_children(parser);
        if (parser->has_error) return node;
        
        // 5. Expect closing tag
        if (check(parser, TOKEN_CLOSE_TAG)) {
            // Check for mismatch
            if (strcmp(parser->current_token.lexeme, node->tag_name) != 0) {
                char msg[256];
                snprintf(msg, sizeof(msg), "Mismatched tag. Expected </%s> but got </%s>",
                         node->tag_name, parser->current_token.lexeme);
                parser_error(parser, msg);
                return node; // Return node, error is flagged
            }
            // All good
            advance(parser); // Consume close tag
            if (!expect(parser, TOKEN_GT, "Expected '>' after closing tag name.")) {
                return node;
            }
        } else {
            char msg[256];
            snprintf(msg, sizeof(msg), "Missing closing tag for <%s>", node->tag_name);
            parser_error(parser, msg);
            return node;
        }
        return node;
    }
    
    // If we get here, something is wrong
    parser_error(parser, "Expected '>' or '/>' after tag attributes.");
    return node;
}