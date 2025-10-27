/**
 * tests/test_lexer.c
 *
 * Unit tests for the Lexer.
 * This is a minimal test suite.
 * In a real project, use a framework like Check or CUnit.
 */
#include <stdio.h>
#include <string.h>
#include "../src/lexer.h"

// Helper function to check a token
// Returns 1 on success, 0 on failure
int check_token(Lexer* lexer, TokenType expected_type, const char* expected_lexeme) {
    Token token = get_next_token(lexer);
    int success = 1;
    
    if (token.type != expected_type) {
        printf("FAIL: Expected token %d, but got %d\n", expected_type, token.type);
        success = 0;
    }
    
    if (expected_lexeme && strcmp(token.lexeme, expected_lexeme) != 0) {
        printf("FAIL: Expected lexeme '%s', but got '%s'\n", expected_lexeme, token.lexeme);
        success = 0;
    }
    
    free_token_lexeme(&token);
    return success;
}

// Public test function
int run_lexer_tests() {
    printf("--- Running Lexer Tests ---\n");
    int success = 1;
    
    const char* test_string = "<p id=\"main\">Hello</p><img src=\"a.jpg\" />";
    Lexer* lexer = lexer_init(test_string);

    if (!check_token(lexer, TOKEN_OPEN_TAG, "p")) success = 0;
    if (!check_token(lexer, TOKEN_ATTR_NAME, "id")) success = 0;
    if (!check_token(lexer, TOKEN_ATTR_EQUALS, "=")) success = 0;
    if (!check_token(lexer, TOKEN_ATTR_VALUE, "main")) success = 0;
    if (!check_token(lexer, TOKEN_GT, ">")) success = 0;
    if (!check_token(lexer, TOKEN_TEXT, "Hello")) success = 0;
    if (!check_token(lexer, TOKEN_CLOSE_TAG, "p")) success = 0;
    if (!check_token(lexer, TOKEN_GT, ">")) success = 0;
    
    if (!check_token(lexer, TOKEN_OPEN_TAG, "img")) success = 0;
    if (!check_token(lexer, TOKEN_ATTR_NAME, "src")) success = 0;
    if (!check_token(lexer, TOKEN_ATTR_EQUALS, "=")) success = 0;
    if (!check_token(lexer, TOKEN_ATTR_VALUE, "a.jpg")) success = 0;
    if (!check_token(lexer, TOKEN_SELF_CLOSE, "/>")) success = 0;
    
    if (!check_token(lexer, TOKEN_EOF, "")) success = 0;

    lexer_free(lexer);
    
    if(success) {
        printf("Lexer Tests: PASS\n");
    } else {
        printf("Lexer Tests: FAIL\n");
    }
    printf("---------------------------\n");
    return success;
}
