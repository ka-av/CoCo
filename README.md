## Overview

A modular compiler front-end implemented in C.
It includes a DFA-based lexical analyzer, automated FIRST and FOLLOW
computation, LL(1) parse table construction, parse tree generation, and
panic-mode error recovery.

------------------------------------------------------------------------

## Features

-   Manual DFA-based lexer (no generator tools used)
-   Twin-buffer input handling
-   Structured token metadata storage
-   Automated FIRST & FOLLOW computation
-   LL(1) parsing with parse table
-   Panic-mode error recovery
-   Parse tree construction and formatted printing
-   GCC compatible
-   Makefile supported

------------------------------------------------------------------------

## Project Structure

CoCo/ ├── lexer.c ├── parser.c ├── driver.c ├── grammar.txt ├── Makefile
└── README.md

------------------------------------------------------------------------

## Lexer Design

### Pattern Matching

-   Deterministic Finite Automaton (DFA)
-   Implemented manually using if-else chains and loops

### Keyword Handling

-   Static keyword table
-   Linear search using strcmp()
-   No hash function used

### Twin Buffer

-   Implemented for efficient file scanning
-   Supports retract and lookahead

### Token Structure

Each token stores: - lexeme (max 64 chars) - TokenType enum - line
number - union for int/real values - isNumber flag (0=none, 1=int,
2=real)

### Lexer Errors Handled

-   Unknown symbols (\$, single =, standalone !)
-   Invalid patterns (&&, \<--)
-   Malformed real numbers
-   Identifier length violations
-   Invalid function/record identifier formats

------------------------------------------------------------------------

## Parser Design

### Grammar Representation

-   Loaded from grammar.txt
-   Stored as array of GrammarRule structures
-   LHS: NonTerminal
-   RHS: array of GrammarSymbols

### FIRST and FOLLOW Sets

-   Automatically computed
-   BitSet representation
-   Dedicated epsilon bit
-   Functions: computeFirst(), computeFollow()

### Parse Table

entries\[NUM_NONTERMINALS\]\[NUM_TOKENS\]

Each cell contains: - Rule index (\>= 0) - PARSE_TABLE_ERROR (-1) -
PARSE_TABLE_SYNCH (-2)

### Parse Tree

-   Implemented as dynamic n-ary tree
-   Parent pointer tracking
-   Rule index stored per node
-   Inorder traversal printing: leftmost child → parent → remaining
    children

Each node prints: - lexeme - line number - token name - numeric value
(if any) - parent symbol - isLeaf flag - node symbol

------------------------------------------------------------------------

## Error Recovery (Parser)

Panic-mode recovery using FOLLOW sets.

Cases handled: 1. Terminal mismatch → pop terminal 2. Parse table error
→ skip input token 3. SYNCH entry → pop non-terminal 4. Nullable
non-terminals → resolve to epsilon silently

------------------------------------------------------------------------

## Compilation

Using Makefile: make

Manual compilation: gcc driver.c lexer.c parser.c -o coco

Compatible with specified GCC version.

------------------------------------------------------------------------

## Execution

    ./coco <inputfile>

Example: ./coco testcase1.txt

------------------------------------------------------------------------

## Execution Status

-   Lexer fully functional
-   Parser fully functional
-   No segmentation faults on official test cases
-   All C files compile successfully

------------------------------------------------------------------------

## Limitations

None. All specified language constructs are supported.
