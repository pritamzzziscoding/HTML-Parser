#include "lexer.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
Lexer* lexer_init(const char* source) {
    Lexer* lexer = (Lexer*)safe_malloc(sizeof(Lexer));
    lexer->source = source;
    lexer->start = 0;
    lexer->current = 0;
    lexer->line = 1;
    lexer->col = 1;
    lexer->insideTag = 0;
    return lexer;
}
void lexer_free(Lexer* lexer) {
    if (lexer) {
        free(lexer);
    }
}

static Token make_token(Lexer* lexer, TokenType type) {
    Token token;
    token.type = type;
    int length = lexer->current - lexer->start;
    char* lexeme = (char*)safe_malloc(length + 1);
    strncpy(lexeme, lexer->source + lexer->start, length);
    lexeme[length] = '\0';
    token.lexeme = lexeme;
    token.line = lexer->line;
    token.col = lexer->col - length;
    
    return token;
}

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
            case '\n':
                advance(lexer);
                break;
            case '<':
                if (peek_next(lexer) == '!') {
                    if (lexer->source[lexer->current + 2] == '-' && lexer->source[lexer->current + 3] == '-') {
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
                            advance(lexer);
                            advance(lexer);
                            advance(lexer);
                        }
                        break;
                    }
                }
            default:
                return;
        }
    }
}

static Token scan_inside_tag(Lexer* lexer) {
    lexer->start = lexer->current;
    char c = advance(lexer);

    switch (c) {
        case '>': return make_token(lexer, TOKEN_GT);
        case '=': return make_token(lexer, TOKEN_ATTR_EQUALS);
        case '/':
            if (peek(lexer) == '>') {
                advance(lexer);
                return make_token(lexer, TOKEN_SELF_CLOSE);
            }
            break;
            
        case '"':
        case '\'':
            while (peek(lexer) != c && !is_at_end(lexer)) {
                advance(lexer);
            }
            if (is_at_end(lexer)) {
                return error_token(lexer, "Unterminated string literal.");
            }
            advance(lexer);
            return make_token(lexer, TOKEN_ATTR_VALUE);

        default:
            if (isalpha(c)) {
                while (isalnum(peek(lexer)) || peek(lexer) == '-') {
                    advance(lexer);
                }
                return make_token(lexer, TOKEN_ATTR_NAME);
            }
    }
    
    return error_token(lexer, "Unexpected character inside tag.");
}
static Token scan_outside_tag(Lexer* lexer) {
    lexer->start = lexer->current;
    if (peek(lexer) == '<') {
        advance(lexer); 
        if (peek(lexer) == '/') {
            advance(lexer);
            lexer->start = lexer->current; 
            while (isalpha(peek(lexer))) {
                advance(lexer);
            }
            return make_token(lexer, TOKEN_CLOSE_TAG);
        }
        
        if (isalpha(peek(lexer))) {
            // Open tag: <tag
            lexer->start = lexer->current;
            while (isalpha(peek(lexer))) {
                advance(lexer);
            }
            return make_token(lexer, TOKEN_OPEN_TAG);
        }
        
        return error_token(lexer, "Invalid tag syntax.");
    }
    while (peek(lexer) != '<' && !is_at_end(lexer)) {
        advance(lexer);
    }
    
    if (lexer->current > lexer->start) {
        return make_token(lexer, TOKEN_TEXT);
    }
    return make_token(lexer, TOKEN_EOF);
}
Token get_next_token(Lexer* lexer) {
    skip_whitespace(lexer);
    lexer->start = lexer->current;

    if (is_at_end(lexer)) {
        return make_token(lexer, TOKEN_EOF);
    }
    if (lexer->insideTag) {
        char c = peek(lexer);

        if (c == '>') {
            advance(lexer);
            lexer->insideTag = 0;
            return make_token(lexer, TOKEN_GT);
        } 
        else if (c == '=') {
            advance(lexer);
            return make_token(lexer, TOKEN_ATTR_EQUALS);
        } 
        else if (c == '"' || c == '\'') {
            advance(lexer);
            lexer->start = lexer->current;
            while (peek(lexer) != c && !is_at_end(lexer)) {
                advance(lexer);
            }
            if (is_at_end(lexer)) return error_token(lexer, "Unterminated string.");
            Token tok = make_token(lexer, TOKEN_ATTR_VALUE);
            advance(lexer);
            return tok;
        } 
        else if (isalpha(c)) {
            while (isalnum(peek(lexer)) || peek(lexer) == '-') {
                advance(lexer);
            }
            return make_token(lexer, TOKEN_ATTR_NAME);
        } 
        else if (c == '/' && peek_next(lexer) == '>') {
            advance(lexer);
            advance(lexer); 
            lexer->insideTag = 0;
            return make_token(lexer, TOKEN_SELF_CLOSE);
        }

        return error_token(lexer, "Unexpected char inside tag.");
    }
    char c = peek(lexer);
    if (c == '<') {
        advance(lexer);
        if (peek(lexer) == '/') {
            advance(lexer);
            lexer->start = lexer->current;
            while (isalnum(peek(lexer)) || peek(lexer) == '-') advance(lexer);
            lexer->insideTag = 1;
            return make_token(lexer, TOKEN_CLOSE_TAG);
        } else if (isalpha(peek(lexer))) {
            lexer->start = lexer->current;
            while (isalnum(peek(lexer)) || peek(lexer) == '-') advance(lexer);
            lexer->insideTag = 1;
            return make_token(lexer, TOKEN_OPEN_TAG);
        } else {
            return error_token(lexer, "Invalid tag start.");
        }
    }
    while (peek(lexer) != '<' && !is_at_end(lexer)) {
        advance(lexer);
    }
    return make_token(lexer, TOKEN_TEXT);
}