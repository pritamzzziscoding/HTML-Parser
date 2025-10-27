/**
 * tests/test_runner.c
 *
 * Main entry point for running all unit tests.
 *
 * This file is compiled *instead of* src/main.c to create a
 * 'run_tests' executable.
 */
#include <stdio.h>

// Include declarations for the test functions
int run_lexer_tests();
int run_parser_tests();

int main() {
    printf("========= HTML PARSER TEST SUITE =========\n\n");
    
    int lexer_success = run_lexer_tests();
    int parser_success = run_parser_tests();
    
    printf("\n================= SUMMARY ==================\n");
    printf("Lexer Tests:  %s\n", lexer_success  ? "PASS" : "FAIL");
    printf("Parser Tests: %s\n", parser_success ? "PASS" : "FAIL");
    printf("==========================================\n");
    
    // Return 0 if all tests passed, 1 otherwise
    return (lexer_success && parser_success) ? 0 : 1;
}