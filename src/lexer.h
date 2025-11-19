#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_OPEN_TAG,     // <tag
    TOKEN_CLOSE_TAG,    // </tag
    TOKEN_SELF_CLOSE,   // />
    TOKEN_GT,           // >
    TOKEN_TEXT,         // Any raw text content
    TOKEN_ATTR_NAME,    // attribute_name
    TOKEN_ATTR_EQUALS,  // =
    TOKEN_ATTR_VALUE,   // "value" or 'value'
    TOKEN_ERROR,        // An unrecognized character or syntax
    TOKEN_EOF           // End of file
} TokenType;

typedef struct {
    TokenType type;
    char* lexeme;      
    int line;        
    int col; 
} Token;

typedef struct {
    const char* source;
    int start; 
    int current;
    int line;
    int col;
    int insideTag;
} Lexer;



Lexer* lexer_init(const char* source);

void lexer_free(Lexer* lexer);


Token get_next_token(Lexer* lexer);

void free_token_lexeme(Token* token);

#endif
