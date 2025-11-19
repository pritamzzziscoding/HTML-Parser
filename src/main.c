#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "dom.h"

static void delay_print(const char *message, int ms_delay) {
    printf("%s", message);
    fflush(stdout);
    usleep(ms_delay * 1000); 
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename.html>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char* filename = argv[1];

    printf(" Starting HTML Parser...\n\n");
    sleep(1);
    printf(" Reading file: %s\n", filename);
    delay_print("   Loading", 300);
    for (int i = 0; i < 3; i++) {
        delay_print(".", 400);
    }
    printf("\n");

    char* source_code = read_file_to_buffer(filename);
    if (source_code == NULL) {
        fprintf(stderr, " Error: Could not read file '%s'.\n", filename);
        return EXIT_FAILURE;
    }
    printf(" File read successfully (%ld bytes)\n\n", strlen(source_code));
    sleep(1);
    printf(" Initializing Lexer and Parser...\n");
    Lexer* lexer = lexer_init(source_code);
    Parser* parser = parser_init(lexer);
    usleep(800 * 1000);
    printf(" Components initialized successfully.\n\n");
    sleep(1);
    printf(" Parsing document...\n");
    for (int i = 0; i < 3; i++) {
        delay_print("   Processing", 350);
        for (int j = 0; j < 3; j++) delay_print(".", 300);
        printf("\r");
    }
    printf("\n");

    DomNode* dom_root = parse(parser);
    if (parser->has_error) {
        fprintf(stderr, "\n PARSE FAILED \n");
        fprintf(stderr, "%s\n", parser->error_message);
        free(source_code);
        lexer_free(lexer);
        parser_free(parser);
        free_dom_tree(dom_root);
        return EXIT_FAILURE;
    }

    printf(" Parsing successful!\n\n");
    sleep(1);
    printf(" Generated DOM Tree:\n\n");
    usleep(700 * 1000);
    print_dom_tree(dom_root, 0);
    printf("\n--------------------------\n\n");
    sleep(1);

    printf(" Cleaning up memory...\n");
    for (int i = 0; i < 3; i++) {
        delay_print("   Freeing resources", 400);
        for (int j = 0; j < 3; j++) delay_print(".", 300);
        printf("\r");
    }
    printf("\n Done.\n");

    free(source_code);
    lexer_free(lexer);
    parser_free(parser);
    free_dom_tree(dom_root);

    printf("\n Program finished successfully! \n");
    return EXIT_SUCCESS;
}
