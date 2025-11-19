#ifndef DOM_H
#define DOM_H


typedef enum {
    ELEMENT_NODE,
    TEXT_NODE
} NodeType;

typedef struct Attribute {
    char* name;
    char* value;
    struct Attribute* next;
} Attribute;


typedef struct DomNode {
    NodeType type;

    char* tag_name;
    Attribute* attributes;

    char* text_content;

    struct DomNode* parent;
    struct DomNode* first_child;
    struct DomNode* next_sibling;

} DomNode;


DomNode* create_element_node(const char* tag_name);

DomNode* create_text_node(const char* text);

void add_child(DomNode* parent, DomNode* child);

void add_attribute(DomNode* node, const char* name, const char* value);

void free_dom_tree(DomNode* root);

void print_dom_tree(DomNode* root, int indent);

#endif // DOM_H
