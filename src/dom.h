/**
 * src/dom.h
 *
 * Defines the data structures for the Document Object Model (DOM) tree.
 * This is the output of our parser.
 */
#ifndef DOM_H
#define DOM_H

// Type of DOM node
typedef enum {
    ELEMENT_NODE,
    TEXT_NODE
} NodeType;

// A linked list of attributes (e.g., href="style.css")
typedef struct Attribute {
    char* name;
    char* value;
    struct Attribute* next;
} Attribute;

// The core DOM Node structure
typedef struct DomNode {
    NodeType type;

    // For ELEMENT_NODE
    char* tag_name;
    Attribute* attributes;

    // For TEXT_NODE
    char* text_content;

    // Tree hierarchy pointers
    struct DomNode* parent;
    struct DomNode* first_child;
    struct DomNode* next_sibling;

} DomNode;

// --- Public DOM Functions ---

/**
 * @brief Creates a new element node (e.g., <p>, <div>).
 * @param tag_name The name of the tag (e.g., "p").
 * @return A pointer to the new DomNode.
 */
DomNode* create_element_node(const char* tag_name);

/**
 * @brief Creates a new text node (e.g., "Hello, world!").
 * @param text The text content of the node.
 * @return A pointer to the new DomNode.
 */
DomNode* create_text_node(const char* text);

/**
 * @brief Appends a child node to a parent node.
 * Manages the first_child and next_sibling pointers.
 * @param parent The parent node.
 * @param child The child node to add.
 */
void add_child(DomNode* parent, DomNode* child);

/**
 * @brief Adds an attribute to an element node.
 * @param node The element node.
 * @param name The attribute's name (e.g., "href").
 * @param value The attribute's value (e.g., "index.html").
 */
void add_attribute(DomNode* node, const char* name, const char* value);

/**
 * @brief Recursively frees an entire DOM subtree.
 * @param root The root node of the tree to free.
 */
void free_dom_tree(DomNode* root);

/**
 * @brief Prints a representation of the DOM tree to stdout.
 * @param root The root of the tree to print.
 * @param indent The current indentation level (start with 0).
 */
void print_dom_tree(DomNode* root, int indent);

#endif // DOM_H
