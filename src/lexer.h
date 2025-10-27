/**
 * src/lexer.h
 *
 * Defines the Lexer (Tokenizer) structures and functions.
 * The Lexer scans the raw HTML source and produces a stream of Tokens.
 */
#ifndef LEXER_H
#define LEXER_H

// --- Token Types ---
// Defines all the "words" our parser understands
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

// --- Token Structure ---
// A single "word"
typedef struct {
    TokenType type;
    char* lexeme;       // The actual text (e.g., "p", "src", "Hello")
    int line;           // Line number for error reporting
    int col;            // Column number for error reporting
} Token;

// --- Lexer State ---
// Manages the tokenization process
typedef struct {
    const char* source; // Pointer to the raw HTML source string
    int start;          // Start of the current lexeme
    int current;        // Current position in the source
    int line;
    int col;
} Lexer;

// --- Public Lexer Functions ---

/**
 * @brief Initializes a new Lexer with the given source code.
 * @param source The HTML source string to tokenize.
 * @return A pointer to the new Lexer.
 */
Lexer* lexer_init(const char* source);

/**
 * @brief Frees the memory associated with the Lexer.
 * @param lexer The Lexer to free.
 */
void lexer_free(Lexer* lexer);

/**
 * @brief Scans and returns the next token from the source.
 * The caller is responsible for freeing the token's lexeme.
 * @param lexer The Lexer.
 * @return The next Token.
 */
Token get_next_token(Lexer* lexer);

/**
 * @brief Frees the 'lexeme' string inside a Token.
 * @param token The token whose lexeme to free.
 */
void free_token_lexeme(Token* token);

#endif // LEXER_H
