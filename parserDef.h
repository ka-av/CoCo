// Group number 34
// ID: 2023A7PS0564P 		Name: Soham Roy
//ID: 2023A7PS0678P 		Name: Kaavya Mer 
//ID: 2023A7PS0614P 		Name: Arindam Verma 
//ID: 2023A7PS0502P 		Name: Mona Gupta
//ID: 2023A7PS0509P 		Name: Tanushree Tokas
//ID: 2023A7PS0516P 		Name: Disha Goel

#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#include "lexerDef.h"

#define MAX_RULES          100
#define MAX_RHS_LEN        20
#define MAX_SYMBOL_NAME    40
#define EPSILON_SYM_ID     -2   
#define DOLLAR_SYM_ID      -3   

typedef enum {
    NT_program = 0,
    NT_mainFunction,
    NT_otherFunctions,
    NT_function,
    NT_input_par,
    NT_output_par,
    NT_parameter_list,
    NT_dataType,
    NT_primitiveDatatype,
    NT_constructedDatatype,
    NT_remaining_list,
    NT_stmts,
    NT_typeDefinitions,
    NT_typeDefinition,
    NT_fieldDefinitions,
    NT_fieldDefinition,
    NT_moreFields,
    NT_declarations,
    NT_declaration,
    NT_global_or_not,
    NT_otherStmts,
    NT_stmt,
    NT_assignmentStmt,
    NT_singleOrRecId,
    NT_option_single_constructed,
    NT_oneExpansion,
    NT_moreExpansions,
    NT_funCallStmt,
    NT_outputParameters,
    NT_inputParameters,
    NT_iterativeStmt,
    NT_conditionalStmt,
    NT_elsePart,
    NT_ioStmt,
    NT_arithmeticExpression,
    NT_expPrime,
    NT_term,
    NT_termPrime,
    NT_factor,
    NT_highPrecedenceOperators,
    NT_lowPrecedenceOperators,
    NT_booleanExpression,
    NT_var,
    NT_logicalOp,
    NT_relationalOp,
    NT_returnStmt,
    NT_optionalReturn,
    NT_idList,
    NT_more_ids,
    NT_definetypestmt,
    NT_A,

    NUM_NONTERMINALS
} NonTerminal;

typedef struct {
    int isTerminal;  
    int id;          
} GrammarSymbol;

typedef struct {
    NonTerminal lhs;
    GrammarSymbol rhs[MAX_RHS_LEN];
    int rhsLen;
} GrammarRule;

typedef struct {
    GrammarRule rules[MAX_RULES];
    int numRules;
} Grammar;

#define BITSET_SIZE  ((NUM_TOKENS + 31) / 32 + 1)  

typedef struct {
    unsigned int bits[BITSET_SIZE];
} BitSet;

typedef struct {
    BitSet firstSet[NUM_NONTERMINALS];
    BitSet followSet[NUM_NONTERMINALS];
} FirstAndFollow;

#define PARSE_TABLE_ERROR  -1
#define PARSE_TABLE_SYNCH  -2

typedef struct {
    int entries[NUM_NONTERMINALS][NUM_TOKENS];
} ParseTable;

typedef struct TreeNode {
    char             lexeme[MAX_LEXEME_LEN];
    TokenType        token;
    int              lineNo;
    union {
        int    intVal;
        double realVal;
    } value;
    int              isNumber; 

    NonTerminal      nonTerminal;

    int              isLeaf;
    struct TreeNode *parent;
    struct TreeNode **children;
    int              numChildren;
    int              ruleIndex;  
} TreeNode;

typedef struct {
    TreeNode *root;
} ParseTree;

#endif 
