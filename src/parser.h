#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "dom.h"
typedef struct {
    Lexer* lexer;
    Token current_token; 
    Token previous_token;
    char* error_message;
    int has_error;
} Parser;

Parser* parser_init(Lexer* lexer);
void parser_free(Parser* parser);

DomNode* parse(Parser* parser);

#endif 
