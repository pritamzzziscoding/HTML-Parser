#include "dom.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

DomNode* create_element_node(const char* tag_name) {
    DomNode* node = (DomNode*)safe_malloc(sizeof(DomNode));
    node->type = ELEMENT_NODE;
    node->tag_name = safe_strdup(tag_name);
    node->text_content = NULL;
    node->attributes = NULL;
    node->parent = NULL;
    node->first_child = NULL;
    node->next_sibling = NULL;
    
    return node;
}

DomNode* create_text_node(const char* text) {
    DomNode* node = (DomNode*)safe_malloc(sizeof(DomNode));
    node->type = TEXT_NODE;
    node->tag_name = NULL;
    node->text_content = safe_strdup(text); 
    node->attributes = NULL;
    node->parent = NULL;
    node->first_child = NULL;
    node->next_sibling = NULL;
    return node;
}

void add_child(DomNode* parent, DomNode* child) {
    if (parent == NULL || child == NULL) {
        return;
    }
    child->parent = parent;

    if (parent->first_child == NULL) {
        parent->first_child = child;
    } else {
        DomNode* current = parent->first_child;
        while (current->next_sibling != NULL) {
            current = current->next_sibling;
        }
        current->next_sibling = child;
    }
}

void add_attribute(DomNode* node, const char* name, const char* value) {
    if (node == NULL || node->type != ELEMENT_NODE || name == NULL || value == NULL) {
        return;
    }

    Attribute* attr = (Attribute*)safe_malloc(sizeof(Attribute));
    attr->name = safe_strdup(name);
    attr->value = safe_strdup(value);
    attr->next = NULL;
    if (node->attributes == NULL) {
        node->attributes = attr;
    } else {
        Attribute* current = node->attributes;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = attr;
    }
}

void free_dom_tree(DomNode* root) {
    if (root == NULL) {
        return;
    }
    free_dom_tree(root->first_child);
    free_dom_tree(root->next_sibling);

    if (root->type == ELEMENT_NODE) {
        free(root->tag_name);
        Attribute* attr = root->attributes;
        while (attr != NULL) {
            Attribute* next_attr = attr->next;
            free(attr->name);
            free(attr->value);
            free(attr);
            attr = next_attr;
        }
    }
    else if (root->type == TEXT_NODE) {
        free(root->text_content);
    }

    free(root);
}

void print_dom_tree(DomNode* root, int indent) {
    if (root == NULL) {
        return;
    }
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    if (root->type == ELEMENT_NODE) {
        printf("|-<%s", root->tag_name);
        Attribute* attr = root->attributes;
        while (attr != NULL) {
            printf(" %s=\"%s\"", attr->name, attr->value);
            attr = attr->next;
        }
        printf(">\n");
        sleep(1);
        DomNode* child = root->first_child;
        while (child != NULL) {
            print_dom_tree(child, indent + 1);
            child = child->next_sibling;
        }

    } else if (root->type == TEXT_NODE) {
        printf("|-TEXT: %s\n", root->text_content);
    }
}

