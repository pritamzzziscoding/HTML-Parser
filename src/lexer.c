/**
 * src/lexer.c
 *
 * Implementation of the Lexer (Tokenizer).
 */
#include "lexer.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Lexer Initialization ---

Lexer* lexer_init(const char* source) {
    Lexer* lexer = (Lexer*)safe_malloc(sizeof(Lexer));
    lexer->source = source;
    lexer->start = 0;
    lexer->current = 0;
    lexer->line = 1;
    lexer->col = 1;
    return lexer;
}

void lexer_free(Lexer* lexer) {
    if (lexer) {
        free(lexer);
    }
}

// --- Token Helper Functions ---

/**
 * @brief Creates a token with the given type.
 * The lexeme is copied from the lexer's source.
 */
static Token make_token(Lexer* lexer, TokenType type) {
    Token token;
    token.type = type;
    
    // Calculate length and copy the lexeme
    int length = lexer->current - lexer->start;
    char* lexeme = (char*)safe_malloc(length + 1);
    strncpy(lexeme, lexer->source + lexer->start, length);
    lexeme[length] = '\0';
    
    token.lexeme = lexeme;
    token.line = lexer->line;
    token.col = lexer->col - length; // Start column of the token
    
    return token;
}

/**
 * @brief Creates an error token with a message.
 */
static Token error_token(Lexer* lexer, const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.lexeme = safe_strdup(message);
    token.line = lexer->line;
    token.col = lexer->col;
    return token;
}

void free_token_lexeme(Token* token) {
    if (token && token->lexeme) {
        free(token->lexeme);
        token->lexeme = NULL;
    }
}

// --- Lexer Scanning Helpers ---

static int is_at_end(Lexer* lexer) {
    return lexer->source[lexer->current] == '\0';
}

static char advance(Lexer* lexer) {
    char c = lexer->source[lexer->current];
    lexer->current++;
    if (c == '\n') {
        lexer->line++;
        lexer->col = 1;
    } else {
        lexer->col++;
    }
    return c;
}

static char peek(Lexer* lexer) {
    return lexer->source[lexer->current];
}

static char peek_next(Lexer* lexer) {
    if (is_at_end(lexer)) return '\0';
    return lexer->source[lexer->current + 1];
}

static void skip_whitespace(Lexer* lexer) {
    while (1) {
        char c = peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
            case '\n': // Also track newlines
                advance(lexer);
                break;
            // Handle HTML comments <!-- ... -->
            case '<':
                if (peek_next(lexer) == '!') {
                    if (lexer->source[lexer->current + 2] == '-' && lexer->source[lexer->current + 3] == '-') {
                        // Start of a comment
                        advance(lexer); // <
                        advance(lexer); // !
                        advance(lexer); // -
                        advance(lexer); // -
                        while (!is_at_end(lexer) && 
                               !(peek(lexer) == '-' && 
                                 peek_next(lexer) == '-' && 
                                 lexer->source[lexer->current + 2] == '>')) {
                            advance(lexer);
                        }
                        if (!is_at_end(lexer)) {
                            advance(lexer); // -
                            advance(lexer); // -
                            advance(lexer); // >
                        }
                        break;
                    }
                }
                // Fallthrough if not a comment
            default:
                return;
        }
    }
}

// --- State-based Scanning Functions ---

// State for when lexer is inside a tag (e.g., <p id="foo">)
static Token scan_inside_tag(Lexer* lexer) {
    lexer->start = lexer->current;
    char c = advance(lexer);

    switch (c) {
        case '>': return make_token(lexer, TOKEN_GT);
        case '=': return make_token(lexer, TOKEN_ATTR_EQUALS);
        case '/':
            if (peek(lexer) == '>') {
                advance(lexer); // Consume '>'
                return make_token(lexer, TOKEN_SELF_CLOSE);
            }
            break; // Not a self-close, maybe an error
            
        case '"':
        case '\'':
            // Attribute value
            while (peek(lexer) != c && !is_at_end(lexer)) {
                advance(lexer);
            }
            if (is_at_end(lexer)) {
                return error_token(lexer, "Unterminated string literal.");
            }
            advance(lexer); // Consume the closing quote
            return make_token(lexer, TOKEN_ATTR_VALUE);

        default:
            // Attribute name
            if (isalpha(c)) {
                while (isalnum(peek(lexer)) || peek(lexer) == '-') {
                    advance(lexer);
                }
                return make_token(lexer, TOKEN_ATTR_NAME);
            }
    }
    
    return error_token(lexer, "Unexpected character inside tag.");
}

// State for when lexer is outside a tag (parsing text or new tags)
static Token scan_outside_tag(Lexer* lexer) {
    lexer->start = lexer->current;

    // 1. Handle tags
    if (peek(lexer) == '<') {
        advance(lexer); // Consume '<'
        
        if (peek(lexer) == '/') {
            // Close tag: </tag>
            advance(lexer); // Consume '/'
            lexer->start = lexer->current; // Mark start of tag name
            while (isalpha(peek(lexer))) {
                advance(lexer);
            }
            return make_token(lexer, TOKEN_CLOSE_TAG);
        }
        
        if (isalpha(peek(lexer))) {
            // Open tag: <tag
            lexer->start = lexer->current; // Mark start of tag name
            while (isalpha(peek(lexer))) {
                advance(lexer);
            }
            return make_token(lexer, TOKEN_OPEN_TAG);
        }
        
        return error_token(lexer, "Invalid tag syntax.");
    }

    // 2. Handle text content
    // Consume text until the next '<' or EOF
    while (peek(lexer) != '<' && !is_at_end(lexer)) {
        advance(lexer);
    }
    
    // We've hit a '<' or EOF. The text is from start to current.
    // Check if we actually consumed any text
    if (lexer->current > lexer->start) {
        return make_token(lexer, TOKEN_TEXT);
    }

    // If we're here, it means we hit '<' immediately, which should be
    // handled by the next call to get_next_token.
    // Or we are at the end of the file.
    return make_token(lexer, TOKEN_EOF);
}


// --- Main get_next_token Function ---

// A simple state machine:
// We are either "outside" a tag (looking for text or <)
// or "inside" a tag (looking for >, attributes, etc.)
// We track this state *outside* the lexer, in the parser.
// The lexer just needs to know what to do based on the *next* char.

Token get_next_token(Lexer* lexer) {
    skip_whitespace(lexer);
    lexer->start = lexer->current;

    if (is_at_end(lexer)) {
        return make_token(lexer, TOKEN_EOF);
    }
    
    char c = peek(lexer);

    if (c == '<') {
        // --- Tag-related tokens ---
        advance(lexer); // Consume '<'
        
        if (peek(lexer) == '/') {
            // Close tag: </tag>
            advance(lexer); // Consume '/'
            lexer->start = lexer->current; // Mark start of tag name
            while (isalnum(peek(lexer)) || peek(lexer) == '-') {
                advance(lexer);
            }
            return make_token(lexer, TOKEN_CLOSE_TAG);
        }
        
        if (isalpha(peek(lexer))) {
            // Open tag: <tag
            lexer->start = lexer->current; // Mark start of tag name
            while (isalnum(peek(lexer)) || peek(lexer) == '-') {
                advance(lexer);
            }
            return make_token(lexer, TOKEN_OPEN_TAG);
        }
        
        return error_token(lexer, "Invalid tag syntax after '<'.");
    } 
    else if (c == '>') {
        // --- Inside-tag tokens ---
        advance(lexer);
        return make_token(lexer, TOKEN_GT);
    }
    else if (c == '=') {
        advance(lexer);
        return make_token(lexer, TOKEN_ATTR_EQUALS);
    }
    else if (c == '/') {
        if (peek_next(lexer) == '>') {
            advance(lexer); // Consume '/'
            advance(lexer); // Consume '>'
            return make_token(lexer, TOKEN_SELF_CLOSE);
        }
        return error_token(lexer, "Unexpected '/'.");
    }
    else if (c == '"' || c == '\'') {
        // Attribute value
        advance(lexer); // Consume opening quote
        lexer->start = lexer->current; // Mark start of value
        while (peek(lexer) != c && !is_at_end(lexer)) {
            advance(lexer);
        }
        if (is_at_end(lexer)) {
            return error_token(lexer, "Unterminated string literal.");
        }
        Token token = make_token(lexer, TOKEN_ATTR_VALUE);
        advance(lexer); // Consume the closing quote
        return token;
    }
    else if (isalpha(c)) {
        // Could be TEXT or ATTR_NAME. We assume the *parser* knows
        // which state it's in. The lexing rule is the same:
        // read a block of text.
        // If it's text, it stops at '<'.
        // If it's an attr_name, it stops at '=' or '>'.
        
        // Let's refine this: We need two "modes" for the lexer.
        // But the parser can control this.
        // Let's assume the parser calls a *different* function
        // `get_text_token()` if it expects text.
        
        // For simplicity, let's make THIS function state-agnostic.
        // If it sees a letter, it must be an ATTR_NAME,
        // because TEXT is handled by the `else` block.
        while (isalnum(peek(lexer)) || peek(lexer) == '-') {
            advance(lexer);
        }
        return make_token(lexer, TOKEN_ATTR_NAME);
    }
    else {
        // --- Text Node ---
        // If it's not a tag or any tag-related character,
        // it must be text content.
        // Read until the next '<' or EOF.
        while (peek(lexer) != '<' && !is_at_end(lexer)) {
            advance(lexer);
        }
        
        if (lexer->current > lexer->start) {
            return make_token(lexer, TOKEN_TEXT);
        }
    }
    
    if (is_at_end(lexer)) {
         return make_token(lexer, TOKEN_EOF);
    }

    return error_token(lexer, "Unexpected character.");
}
