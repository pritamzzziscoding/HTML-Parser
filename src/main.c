/**
 * src/main.c
 *
 * The main entry point for the HTML parser executable.
 *
 * Usage: ./html_parser <filename.html>
 */
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "dom.h"

int main(int argc, char* argv[]) {
    // --- 1. Argument Checking ---
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename.html>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char* filename = argv[1];

    // --- 2. Read File ---
    printf("--- Reading file: %s ---\n", filename);
    char* source_code = read_file_to_buffer(filename);
    if (source_code == NULL) {
        fprintf(stderr, "Error: Could not read file '%s'.\n", filename);
        return EXIT_FAILURE;
    }
    printf("--- File read successfully (%ld bytes) ---\n\n", strlen(source_code));

    // --- 3. Initialize Components ---
    Lexer* lexer = lexer_init(source_code);
    Parser* parser = parser_init(lexer);

    // --- 4. Parse ---
    printf("--- Parsing document... ---\n");
    DomNode* dom_root = parse(parser);

    // --- 5. Check for Errors ---
    if (parser->has_error) {
        fprintf(stderr, "\n--- PARSE FAILED ---\n");
        fprintf(stderr, "%s\n", parser->error_message);
        
        // Cleanup
        free(source_code);
        lexer_free(lexer);
        parser_free(parser);
        free_dom_tree(dom_root); // dom_root might be partially built or NULL
        
        return EXIT_FAILURE;
    }

    printf("--- Parsing successful! ---\n\n");

    // --- 6. Print DOM Tree ---
    printf("--- Generated DOM Tree --- \n");
    print_dom_tree(dom_root, 0);
    printf("-------------------------- \n");

    // --- 7. Cleanup ---
    printf("\n--- Cleaning up memory... ---\n");
    free(source_code);
    lexer_free(lexer);
    parser_free(parser);
    free_dom_tree(dom_root);
    printf("--- Done. ---\n");

    return EXIT_SUCCESS;
}
