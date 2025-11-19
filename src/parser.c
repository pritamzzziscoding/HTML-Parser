#include "parser.h"
#include "utils.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static void advance(Parser* parser);


static int expect(Parser* parser, TokenType type, const char* error_msg);


static int check(Parser* parser, TokenType type);

static DomNode* parse_element(Parser* parser);


static void parse_attributes(Parser* parser, DomNode* node);


static DomNode* parse_children(Parser* parser);


static DomNode* parse_node(Parser* parser);

static int is_self_closing_tag(const char* tag_name);


static void parser_error(Parser* parser, const char* message);


Parser* parser_init(Lexer* lexer) {
    Parser* parser = (Parser*)safe_malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->has_error = 0;
    parser->error_message = NULL;
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
        free(parser);
    }
}

DomNode* parse(Parser* parser) {
    DomNode* root = create_element_node("<!Doctype html>");
    root->first_child = parse_children(parser);

    if (parser->has_error) {
        free_dom_tree(root);
        return NULL;
    }

    return root;
}


static void parser_error(Parser* parser, const char* message) {
    if (parser->has_error) return;
    
    parser->has_error = 1;
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
    free_token_lexeme(&parser->previous_token);

    parser->previous_token = parser->current_token;
    parser->current_token = get_next_token(parser->lexer);
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
        advance(parser); 
        return node;
    }
    return NULL;
}

static DomNode* parse_children(Parser* parser) {
    DomNode* first_child = NULL;
    DomNode* current_child = NULL;
    while (!check(parser, TOKEN_CLOSE_TAG) && !check(parser, TOKEN_EOF)) {
        if (parser->has_error) return first_child;

        DomNode* child_node = parse_node(parser);
        if (child_node == NULL) {
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
            child_node->parent = current_child->parent;
            current_child = child_node;
        }
    }
    
    return first_child;
}

static void parse_attributes(Parser* parser, DomNode* node) {
    while (check(parser, TOKEN_ATTR_NAME)) {
        if (parser->has_error) return;
        char* name = safe_strdup(parser->current_token.lexeme);
        advance(parser); 
        
        char* value = safe_strdup("true");

        if (check(parser, TOKEN_ATTR_EQUALS)) {
            advance(parser); // Consume '='
            if (expect(parser, TOKEN_ATTR_VALUE, "Expected attribute value.")) {
                free(value);
                value = safe_strdup(parser->previous_token.lexeme);
            }
        }
        
        add_attribute(node, name, value);
        free(name);
        free(value);
    }
}

static int is_self_closing_tag(const char* tag_name) {
    const char* self_closing[] = {
        "area", "base", "br", "col", "embed", "hr", "img", 
        "input", "link", "meta", "param", "source", "track", "wbr",
        NULL 
    };
    
    for (int i = 0; self_closing[i]; i++) {
        if (strcmp(tag_name, self_closing[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static DomNode* parse_element(Parser* parser) {
    DomNode* node = create_element_node(parser->current_token.lexeme);
    advance(parser);
    parse_attributes(parser, node);
    if (parser->has_error) return node;
    if (check(parser, TOKEN_SELF_CLOSE)) {
        advance(parser); 
        return node;
    }

    if (check(parser, TOKEN_GT)) {
        advance(parser); 
        if (is_self_closing_tag(node->tag_name)) {
            return node;
        }
        node->first_child = parse_children(parser);
        if (parser->has_error) return node;
        if (check(parser, TOKEN_CLOSE_TAG)) {
            if (strcmp(parser->current_token.lexeme, node->tag_name) != 0) {
                char msg[256];
                snprintf(msg, sizeof(msg), "Mismatched tag. Expected </%s> but got </%s>",
                        node->tag_name, parser->current_token.lexeme);
                parser_error(parser, msg);
                return node; 
            }

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
    parser_error(parser, "Expected '>' or '/>' after tag attributes.");
    return node;
}