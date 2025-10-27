/**
 * src/parser.h
 *
 * Defines the Parser structure and its public interface.
 * The Parser consumes tokens from the Lexer and builds the DOM tree.
 */
#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "dom.h"

// --- Parser State ---
// Manages the parsing process
typedef struct {
    Lexer* lexer;
    Token current_token;  // The token we are currently looking at
    Token previous_token; // The last token we consumed
    
    char* error_message;  // To store error details
    int has_error;
} Parser;

// --- Public Functions ---

/**
 * @brief Initializes a new Parser with the given Lexer.
 * @param lexer The initialized Lexer.
 * @return A pointer to the new Parser.
 */
Parser* parser_init(Lexer* lexer);

/**
 * @brief Frees the memory associated with the Parser.
 * @param parser The Parser to free.
 */
void parser_free(Parser* parser);

/**
 * @brief The main entry point for the parser.
 * Parses the entire token stream and builds the DOM tree.
 * @param parser The Parser.
 * @return The root DomNode of the new DOM tree, or NULL on failure.
 */
DomNode* parse(Parser* parser);

#endif // PARSER_H
