# C HTML Parser

This project is a recursive descent HTML parser written in C, as part of a compiler design project. It includes a lexer, a parser, DOM tree construction, and basic error handling.

## Features

- Lexical Analyzer: Tokenizes HTML into tags, attributes, and text.

- Recursive Descent Parser: Enforces a simple HTML grammar.

- DOM Tree Construction: Builds a tree of DomNode structs.

- Attribute Parsing: Handles name="value" and boolean attributes.

- Self-Closing Tags: Correctly handles both <br> and <br/> syntax.

- Error Handling: Detects and reports mismatched and missing tags.

## Folder Structure
```
html-parser/
├── bin/              # Compiled executables (created by make)
├── obj/              # Object files (created by make)
├── src/              # All .c and .h source files
│   ├── dom.c
│   ├── dom.h
│   ├── lexer.c
│   ├── lexer.h
│   ├── parser.c
│   ├── parser.h
│   ├── utils.c
│   ├── utils.h
│   └── main.c
├── tests/
│   ├── inputs/       # Sample .html files for testing
│   │   ├── test1.html
│   │   ├── ...
│   ├── test_lexer.c
│   ├── test_parser.c
│   └── test_runner.c
├── doc/
│   ├── grammar.txt
│   └── report.md
├── Makefile
└── README.md
```

## How to Build

- You must have gcc and make installed.

- Build the main html_parser executable:

make all


This will create the executable at bin/html_parser.

Build the test runner:

make test


This will create the executable at bin/run_tests.

How to Run

1. Run the Parser

To parse an HTML file, pass it as an argument:

./bin/html_parser tests/inputs/test1.html


Example Output:

--- Reading file: tests/inputs/test1.html ---
--- File read successfully (168 bytes) ---

--- Parsing document... ---
--- Parsing successful! ---

--- Generated DOM Tree ---
<#document>
  ```
  <html>
    <head>
      <title>
        TEXT: "Test Page 1"
    <body>
      <h1>
        TEXT: "Hello, World!"
      <p>
        TEXT: "This is a simple test."
--------------------------
--- Cleaning up memory... ---
--- Done. ---
```

- 2. Run the Unit Tests

To run the built-in test suite:

make run-tests


or just ```./bin/run_tests if already built.```

Example Output:

========= HTML PARSER TEST SUITE =========

--- Running Lexer Tests ---
Lexer Tests: PASS
---------------------------
--- Running Parser Tests ---
  Running test_simple_element...
  ...test_simple_element: PASS
  Running test_self_closing...
  ...test_self_closing: PASS
  Running test_mismatched_tag_error...
  ...test_mismatched_tag_error: PASS (Error detected as expected)
Parser Tests: PASS
--------------------------

================= SUMMARY ==================
Lexer Tests:  PASS
Parser Tests: PASS
==========================================


To Clean

To remove all compiled files (in bin/ and obj/):

make clean