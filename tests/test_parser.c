/**
 * tests/test_parser.c
 *
 * Unit tests for the Parser.
 */
#include <stdio.h>
#include <string.h>
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/dom.h"
#include "../src/utils.h"

// Helper macro for assertions
#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s (at %s:%d)\n", message, __FILE__, __LINE__); \
            return 0; \
        } \
    } while (0)

int test_simple_element() {
    printf("  Running test_simple_element...\n");
    const char* source = "<p>Hello</p>";
    Lexer* lexer = lexer_init(source);
    Parser* parser = parser_init(lexer);
    DomNode* root = parse(parser);

    ASSERT(root != NULL, "Root is NULL");
    ASSERT(!parser->has_error, parser->error_message ? parser->error_message : "Unknown parse error");
    
    DomNode* p = root->first_child;
    ASSERT(p != NULL, "<p> node is NULL");
    ASSERT(p->type == ELEMENT_NODE, "<p> is not ELEMENT_NODE");
    ASSERT(strcmp(p->tag_name, "p") == 0, "Tag name is not 'p'");
    
    DomNode* text = p->first_child;
    ASSERT(text != NULL, "Text node is NULL");
    ASSERT(text->type == TEXT_NODE, "Child is not TEXT_NODE");
    ASSERT(strcmp(text->text_content, "Hello") == 0, "Text content is not 'Hello'");
    
    free_dom_tree(root);
    parser_free(parser);
    lexer_free(lexer);
    printf("  ...test_simple_element: PASS\n");
    return 1;
}

int test_self_closing() {
    printf("  Running test_self_closing...\n");
    const char* source = "<div><br/><img src=\"test.png\"></div>";
    Lexer* lexer = lexer_init(source);
    Parser* parser = parser_init(lexer);
    DomNode* root = parse(parser);

    ASSERT(root != NULL, "Root is NULL");
    ASSERT(!parser->has_error, parser->error_message ? parser->error_message : "Unknown parse error");

    DomNode* div = root->first_child;
    ASSERT(div != NULL, "<div> is NULL");
    ASSERT(strcmp(div->tag_name, "div") == 0, "Tag is not 'div'");
    
    DomNode* br = div->first_child;
    ASSERT(br != NULL, "<br> is NULL");
    ASSERT(strcmp(br->tag_name, "br") == 0, "Tag is not 'br'");
    ASSERT(br->first_child == NULL, "<br> should have no children");

    DomNode* img = br->next_sibling;
    ASSERT(img != NULL, "<img> is NULL");
    ASSERT(strcmp(img->tag_name, "img") == 0, "Tag is not 'img'");
    ASSERT(img->first_child == NULL, "<img> should have no children");
    ASSERT(img->attributes != NULL, "<img> has no attributes");
    ASSERT(strcmp(img->attributes->name, "src") == 0, "Attribute is not 'src'");
    ASSERT(strcmp(img->attributes->value, "test.png") == 0, "Value is not 'test.png'");
    
    free_dom_tree(root);
    parser_free(parser);
    lexer_free(lexer);
    printf("  ...test_self_closing: PASS\n");
    return 1;
}

int test_mismatched_tag_error() {
    printf("  Running test_mismatched_tag_error...\n");
    const char* source = "<b><i>Hello</b></i>";
    Lexer* lexer = lexer_init(source);
    Parser* parser = parser_init(lexer);
    DomNode* root = parse(parser);

    ASSERT(parser->has_error, "Parser did not detect mismatched tag error");
    ASSERT(root == NULL, "Root should be NULL on parse failure");
    
    // We can't free_dom_tree(root) since it's NULL (parse() frees it)
    parser_free(parser);
    lexer_free(lexer);
    printf("  ...test_mismatched_tag_error: PASS (Error detected as expected)\n");
    return 1;
}


// Public test function
int run_parser_tests() {
    printf("--- Running Parser Tests ---\n");
    int success = 1;

    if (!test_simple_element()) success = 0;
    if (!test_self_closing()) success = 0;
    if (!test_mismatched_tag_error()) success = 0;

    if(success) {
        printf("Parser Tests: PASS\n");
    } else {
        printf("Parser Tests: FAIL\n");
    }
    printf("--------------------------\n");
    return success;
}
