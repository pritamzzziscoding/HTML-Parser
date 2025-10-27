/**
 * src/dom.c
 *
 * Implementation of the DOM tree functions.
 */
#include "dom.h"
#include "utils.h" // For safe_malloc and safe_strdup
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Function Implementations ---

DomNode* create_element_node(const char* tag_name) {
    // Allocate memory for the node itself
    DomNode* node = (DomNode*)safe_malloc(sizeof(DomNode));
    
    // Set its properties
    node->type = ELEMENT_NODE;
    node->tag_name = safe_strdup(tag_name); // Copy the tag name
    node->text_content = NULL;
    node->attributes = NULL;
    node->parent = NULL;
    node->first_child = NULL;
    node->next_sibling = NULL;
    
    return node;
}

DomNode* create_text_node(const char* text) {
    // Allocate memory for the node itself
    DomNode* node = (DomNode*)safe_malloc(sizeof(DomNode));
    
    // Set its properties
    node->type = TEXT_NODE;
    node->tag_name = NULL;
    node->text_content = safe_strdup(text); // Copy the text content
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

    // Set the parent of the child
    child->parent = parent;

    // If the parent has no children yet, this becomes the first child
    if (parent->first_child == NULL) {
        parent->first_child = child;
    } else {
        // Otherwise, find the last child and append this one as its sibling
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

    // Create the new attribute
    Attribute* attr = (Attribute*)safe_malloc(sizeof(Attribute));
    attr->name = safe_strdup(name);
    attr->value = safe_strdup(value);
    attr->next = NULL;

    // Add it to the front of the node's attribute list
    // (Alternatively, could append to the end)
    if (node->attributes == NULL) {
        node->attributes = attr;
    } else {
        // Find the end of the list and append
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

    // --- 1. Recursively free children and siblings ---
    free_dom_tree(root->first_child);
    free_dom_tree(root->next_sibling);

    // --- 2. Free the data for this node ---
    
    // Free element-specific data
    if (root->type == ELEMENT_NODE) {
        free(root->tag_name);
        
        // Free the attribute list
        Attribute* attr = root->attributes;
        while (attr != NULL) {
            Attribute* next_attr = attr->next;
            free(attr->name);
            free(attr->value);
            free(attr);
            attr = next_attr;
        }
    }
    // Free text-specific data
    else if (root->type == TEXT_NODE) {
        free(root->text_content);
    }

    // --- 3. Free the node itself ---
    free(root);
}

void print_dom_tree(DomNode* root, int indent) {
    if (root == NULL) {
        return;
    }

    // --- 1. Print this node ---

    // Print indentation
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    if (root->type == ELEMENT_NODE) {
        // Print tag name
        printf("<%s", root->tag_name);
        
        // Print attributes
        Attribute* attr = root->attributes;
        while (attr != NULL) {
            printf(" %s=\"%s\"", attr->name, attr->value);
            attr = attr->next;
        }
        printf(">\n");

        // --- 2. Recursively print children ---
        DomNode* child = root->first_child;
        while (child != NULL) {
            print_dom_tree(child, indent + 1);
            child = child->next_sibling;
        }

    } else if (root->type == TEXT_NODE) {
        // Print text content, escaped
        printf("TEXT: \"%s\"\n", root->text_content);
    }

    // --- 3. Recursively print siblings (handled by the parent's loop) ---
}
