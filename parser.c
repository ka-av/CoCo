// Group number 34
// ID: 2023A7PS0564P 		Name: Soham Roy
//ID: 2023A7PS0678P 		Name: Kaavya Mer 
//ID: 2023A7PS0614P 		Name: Arindam Verma 
//ID: 2023A7PS0502P 		Name: Mona Gupta
//ID: 2023A7PS0509P 		Name: Tanushree Tokas
//ID: 2023A7PS0516P 		Name: Disha Goel

#include "parserDef.h"
#include "lexer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const char *nonTerminalNames[] = {
    "program",
    "mainFunction",
    "otherFunctions",
    "function",
    "input_par",
    "output_par",
    "parameter_list",
    "dataType",
    "primitiveDatatype",
    "constructedDatatype",
    "remaining_list",
    "stmts",
    "typeDefinitions",
    "typeDefinition",
    "fieldDefinitions",
    "fieldDefinition",
    "moreFields",
    "declarations",
    "declaration",
    "global_or_not",
    "otherStmts",
    "stmt",
    "assignmentStmt",
    "singleOrRecId",
    "option_single_constructed",
    "oneExpansion",
    "moreExpansions",
    "funCallStmt",
    "outputParameters",
    "inputParameters",
    "iterativeStmt",
    "conditionalStmt",
    "elsePart",
    "ioStmt",
    "arithmeticExpression",
    "expPrime",
    "term",
    "termPrime",
    "factor",
    "highPrecedenceOperators",
    "lowPrecedenceOperators",
    "booleanExpression",
    "var",
    "logicalOp",
    "relationalOp",
    "returnStmt",
    "optionalReturn",
    "idList",
    "more_ids",
    "definetypestmt",
    "A"
};

const char *getNonTerminalName(NonTerminal nt) {
    if (nt >= 0 && nt < NUM_NONTERMINALS)
        return nonTerminalNames[nt];
    return "UNKNOWN_NT";
}

#define EPSILON_BIT  NUM_TOKENS
#define DOLLAR_BIT   TK_EOF    

static void bitsetClear(BitSet *s) {
    memset(s->bits, 0, sizeof(s->bits));
}

static void bitsetAdd(BitSet *s, int bit) {
    if (bit < 0) return;
    s->bits[bit / 32] |= (1u << (bit % 32));
}

static int bitsetContains(BitSet *s, int bit) {
    if (bit < 0) return 0;
    return (s->bits[bit / 32] & (1u << (bit % 32))) != 0;
}

static int bitsetUnion(BitSet *dest, BitSet *src) {
    int changed = 0;
    for (int i = 0; i < BITSET_SIZE; i++) {
        unsigned int old = dest->bits[i];
        dest->bits[i] |= src->bits[i];
        if (dest->bits[i] != old) changed = 1;
    }
    return changed;
}

static int bitsetUnionWithoutEpsilon(BitSet *dest, BitSet *src) {
    int changed = 0;
    BitSet temp;
    memcpy(&temp, src, sizeof(BitSet));
    temp.bits[EPSILON_BIT / 32] &= ~(1u << (EPSILON_BIT % 32));
    changed = bitsetUnion(dest, &temp);
    return changed;
}

static GrammarSymbol makeTerminal(TokenType t) {
    GrammarSymbol s;
    s.isTerminal = 1;
    s.id = t;
    return s;
}

static GrammarSymbol makeNonTerminal(NonTerminal nt) {
    GrammarSymbol s;
    s.isTerminal = 0;
    s.id = nt;
    return s;
}

static GrammarSymbol makeEpsilon(void) {
    GrammarSymbol s;
    s.isTerminal = 1;
    s.id = EPSILON_SYM_ID;
    return s;
}

typedef struct { const char *name; TokenType token; } TerminalEntry;

static TerminalEntry terminalLookup[] = {
    {"TK_ASSIGNOP", TK_ASSIGNOP}, {"TK_PLUS", TK_PLUS}, {"TK_MINUS", TK_MINUS},
    {"TK_MUL", TK_MUL}, {"TK_DIV", TK_DIV}, {"TK_LT", TK_LT}, {"TK_LE", TK_LE},
    {"TK_GT", TK_GT}, {"TK_GE", TK_GE}, {"TK_EQ", TK_EQ}, {"TK_NE", TK_NE},
    {"TK_AND", TK_AND}, {"TK_OR", TK_OR}, {"TK_NOT", TK_NOT},
    {"TK_SQL", TK_SQL}, {"TK_SQR", TK_SQR}, {"TK_OP", TK_OP}, {"TK_CL", TK_CL},
    {"TK_COMMA", TK_COMMA}, {"TK_SEM", TK_SEM}, {"TK_COLON", TK_COLON}, {"TK_DOT", TK_DOT},
    {"TK_ID", TK_ID}, {"TK_FUNID", TK_FUNID}, {"TK_RUID", TK_RUID}, {"TK_FIELDID", TK_FIELDID},
    {"TK_NUM", TK_NUM}, {"TK_RNUM", TK_RNUM},
    {"TK_WITH", TK_WITH}, {"TK_PARAMETERS", TK_PARAMETERS}, {"TK_END", TK_END},
    {"TK_WHILE", TK_WHILE}, {"TK_UNION", TK_UNION}, {"TK_ENDUNION", TK_ENDUNION},
    {"TK_DEFINETYPE", TK_DEFINETYPE}, {"TK_AS", TK_AS}, {"TK_TYPE", TK_TYPE},
    {"TK_MAIN", TK_MAIN}, {"TK_GLOBAL", TK_GLOBAL}, {"TK_PARAMETER", TK_PARAMETER},
    {"TK_LIST", TK_LIST}, {"TK_INPUT", TK_INPUT}, {"TK_OUTPUT", TK_OUTPUT},
    {"TK_INT", TK_INT}, {"TK_REAL", TK_REAL}, {"TK_ENDWHILE", TK_ENDWHILE},
    {"TK_IF", TK_IF}, {"TK_THEN", TK_THEN}, {"TK_ENDIF", TK_ENDIF},
    {"TK_READ", TK_READ}, {"TK_WRITE", TK_WRITE}, {"TK_RETURN", TK_RETURN},
    {"TK_CALL", TK_CALL}, {"TK_RECORD", TK_RECORD}, {"TK_ENDRECORD", TK_ENDRECORD},
    {"TK_ELSE", TK_ELSE}, {"TK_EOF", TK_EOF},
    {NULL, TK_ERR}
};

typedef struct { const char *name; NonTerminal nt; } NonTerminalEntry;

static NonTerminalEntry nonTerminalLookup[] = {
    {"program", NT_program}, {"mainFunction", NT_mainFunction},
    {"otherFunctions", NT_otherFunctions}, {"function", NT_function},
    {"input_par", NT_input_par}, {"output_par", NT_output_par},
    {"parameter_list", NT_parameter_list}, {"dataType", NT_dataType},
    {"primitiveDatatype", NT_primitiveDatatype}, {"constructedDatatype", NT_constructedDatatype},
    {"remaining_list", NT_remaining_list}, {"stmts", NT_stmts},
    {"typeDefinitions", NT_typeDefinitions}, {"typeDefinition", NT_typeDefinition},
    {"fieldDefinitions", NT_fieldDefinitions}, {"fieldDefinition", NT_fieldDefinition},
    {"moreFields", NT_moreFields}, {"declarations", NT_declarations},
    {"declaration", NT_declaration}, {"global_or_not", NT_global_or_not},
    {"otherStmts", NT_otherStmts}, {"stmt", NT_stmt},
    {"assignmentStmt", NT_assignmentStmt}, {"singleOrRecId", NT_singleOrRecId},
    {"option_single_constructed", NT_option_single_constructed},
    {"oneExpansion", NT_oneExpansion}, {"moreExpansions", NT_moreExpansions},
    {"funCallStmt", NT_funCallStmt}, {"outputParameters", NT_outputParameters},
    {"inputParameters", NT_inputParameters}, {"iterativeStmt", NT_iterativeStmt},
    {"conditionalStmt", NT_conditionalStmt}, {"elsePart", NT_elsePart},
    {"ioStmt", NT_ioStmt}, {"arithmeticExpression", NT_arithmeticExpression},
    {"expPrime", NT_expPrime}, {"term", NT_term}, {"termPrime", NT_termPrime},
    {"factor", NT_factor}, {"highPrecedenceOperators", NT_highPrecedenceOperators},
    {"lowPrecedenceOperators", NT_lowPrecedenceOperators},
    {"booleanExpression", NT_booleanExpression}, {"var", NT_var},
    {"logicalOp", NT_logicalOp}, {"relationalOp", NT_relationalOp},
    {"returnStmt", NT_returnStmt}, {"optionalReturn", NT_optionalReturn},
    {"idList", NT_idList}, {"more_ids", NT_more_ids},
    {"definetypestmt", NT_definetypestmt}, {"A", NT_A},
    {NULL, (NonTerminal)-1}
};

static int lookupTerminal(const char *name, TokenType *out) {
    for (int i = 0; terminalLookup[i].name != NULL; i++) {
        if (strcmp(terminalLookup[i].name, name) == 0) {
            *out = terminalLookup[i].token;
            return 1;
        }
    }
    return 0;
}

static int lookupNonTerminal(const char *name, NonTerminal *out) {
    for (int i = 0; nonTerminalLookup[i].name != NULL; i++) {
        if (strcmp(nonTerminalLookup[i].name, name) == 0) {
            *out = nonTerminalLookup[i].nt;
            return 1;
        }
    }
    return 0;
}

#define GRAMMAR_FILE "grammar.txt"
#define MAX_LINE_LEN 512

void initGrammar(Grammar *G) {
    FILE *fp = fopen(GRAMMAR_FILE, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open grammar file '%s'\n", GRAMMAR_FILE);
        G->numRules = 0;
        return;
    }

    int r = 0;
    char line[MAX_LINE_LEN];

    while (fgets(line, sizeof(line), fp) != NULL) {
        int len = (int)strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';

        if (len == 0) continue;

        char *sep = strstr(line, "====>");
        if (sep == NULL) continue;

        *sep = '\0';
        char *lhsStr = line;
        char *rhsStr = sep + 5; 

        while (*lhsStr == ' ' || *lhsStr == '\t') lhsStr++;
        char lhsName[MAX_SYMBOL_NAME];
        int li = 0;
        for (int i = 0; lhsStr[i] != '\0'; i++) {
            if (lhsStr[i] != '<' && lhsStr[i] != '>' && lhsStr[i] != ' ' && lhsStr[i] != '\t') {
                lhsName[li++] = lhsStr[i];
            }
        }
        lhsName[li] = '\0';

        NonTerminal lhsNT;
        if (!lookupNonTerminal(lhsName, &lhsNT)) continue;

        if (r >= MAX_RULES) break;

        G->rules[r].lhs = lhsNT;
        G->rules[r].rhsLen = 0;

        char *token = strtok(rhsStr, " \t");
        while (token != NULL) {
            if (strlen(token) == 0) {
                token = strtok(NULL, " \t");
                continue;
            }

            if (strcmp(token, "eps") == 0) {
                G->rules[r].rhs[G->rules[r].rhsLen++] = makeEpsilon();
            } else if (token[0] == '<') {
                char ntName[MAX_SYMBOL_NAME];
                int ni = 0;
                for (int i = 0; token[i] != '\0'; i++) {
                    if (token[i] != '<' && token[i] != '>') {
                        ntName[ni++] = token[i];
                    }
                }
                ntName[ni] = '\0';

                NonTerminal nt;
                if (lookupNonTerminal(ntName, &nt)) {
                    G->rules[r].rhs[G->rules[r].rhsLen++] = makeNonTerminal(nt);
                }
            } else if (strncmp(token, "TK_", 3) == 0) {
                TokenType tk;
                if (lookupTerminal(token, &tk)) {
                    G->rules[r].rhs[G->rules[r].rhsLen++] = makeTerminal(tk);
                }
            }
            token = strtok(NULL, " \t");
        }
        r++;
    }
    G->numRules = r;
    fclose(fp);
}

static void computeFirst(Grammar *G, FirstAndFollow *F) {
    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        bitsetClear(&F->firstSet[i]);
    }
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int r = 0; r < G->numRules; r++) {
            NonTerminal lhs = G->rules[r].lhs;
            GrammarSymbol *rhs = G->rules[r].rhs;
            int rhsLen = G->rules[r].rhsLen;

            if (rhsLen == 1 && rhs[0].isTerminal && rhs[0].id == EPSILON_SYM_ID) {
                if (!bitsetContains(&F->firstSet[lhs], EPSILON_BIT)) {
                    bitsetAdd(&F->firstSet[lhs], EPSILON_BIT);
                    changed = 1;
                }
                continue;
            }

            int allEpsilon = 1;
            for (int i = 0; i < rhsLen; i++) {
                if (rhs[i].isTerminal) {
                    if (rhs[i].id == EPSILON_SYM_ID) continue;
                    if (!bitsetContains(&F->firstSet[lhs], rhs[i].id)) {
                        bitsetAdd(&F->firstSet[lhs], rhs[i].id);
                        changed = 1;
                    }
                    allEpsilon = 0;
                    break;
                } else {
                    int c2 = bitsetUnionWithoutEpsilon(&F->firstSet[lhs], &F->firstSet[rhs[i].id]);
                    if (c2) changed = 1;
                    if (!bitsetContains(&F->firstSet[rhs[i].id], EPSILON_BIT)) {
                        allEpsilon = 0;
                        break;
                    }
                }
            }
            if (allEpsilon) {
                if (!bitsetContains(&F->firstSet[lhs], EPSILON_BIT)) {
                    bitsetAdd(&F->firstSet[lhs], EPSILON_BIT);
                    changed = 1;
                }
            }
        }
    }
}

static void computeFollow(Grammar *G, FirstAndFollow *F) {
    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        bitsetClear(&F->followSet[i]);
    }
    bitsetAdd(&F->followSet[NT_program], DOLLAR_BIT);
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int r = 0; r < G->numRules; r++) {
            NonTerminal lhs = G->rules[r].lhs;
            GrammarSymbol *rhs = G->rules[r].rhs;
            int rhsLen = G->rules[r].rhsLen;
            for (int i = 0; i < rhsLen; i++) {
                if (rhs[i].isTerminal) continue;
                if (rhs[i].id == EPSILON_SYM_ID) continue;
                NonTerminal B = (NonTerminal)rhs[i].id;
                int allEpsilonAfter = 1;
                for (int j = i + 1; j < rhsLen; j++) {
                    if (rhs[j].isTerminal) {
                        if (rhs[j].id == EPSILON_SYM_ID) continue;
                        if (!bitsetContains(&F->followSet[B], rhs[j].id)) {
                            bitsetAdd(&F->followSet[B], rhs[j].id);
                            changed = 1;
                        }
                        allEpsilonAfter = 0;
                        break;
                    } else {
                        int c2 = bitsetUnionWithoutEpsilon(&F->followSet[B], &F->firstSet[rhs[j].id]);
                        if (c2) changed = 1;
                        if (!bitsetContains(&F->firstSet[rhs[j].id], EPSILON_BIT)) {
                            allEpsilonAfter = 0;
                            break;
                        }
                    }
                }
                if (allEpsilonAfter) {
                    int c2 = bitsetUnion(&F->followSet[B], &F->followSet[lhs]);
                    if (c2) changed = 1;
                }
            }
        }
    }
}

FirstAndFollow ComputeFirstAndFollowSets(Grammar *G) {
    FirstAndFollow F;
    computeFirst(G, &F);
    computeFollow(G, &F);
    return F;
}

static void computeFirstOfString(GrammarSymbol *symbols, int len, FirstAndFollow *F, BitSet *result) {
    bitsetClear(result);
    if (len == 0) {
        bitsetAdd(result, EPSILON_BIT);
        return;
    }
    int allEpsilon = 1;
    for (int i = 0; i < len; i++) {
        if (symbols[i].isTerminal) {
            if (symbols[i].id == EPSILON_SYM_ID) continue;
            bitsetAdd(result, symbols[i].id);
            allEpsilon = 0;
            break;
        } else {
            bitsetUnionWithoutEpsilon(result, &F->firstSet[symbols[i].id]);
            if (!bitsetContains(&F->firstSet[symbols[i].id], EPSILON_BIT)) {
                allEpsilon = 0;
                break;
            }
        }
    }
    if (allEpsilon) bitsetAdd(result, EPSILON_BIT);
}

void createParseTable(FirstAndFollow *F, ParseTable *T, Grammar *G) {
    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        for (int j = 0; j < NUM_TOKENS; j++) T->entries[i][j] = PARSE_TABLE_ERROR;
    }
    for (int r = 0; r < G->numRules; r++) {
        NonTerminal lhs = G->rules[r].lhs;
        BitSet firstOfRHS;
        computeFirstOfString(G->rules[r].rhs, G->rules[r].rhsLen, F, &firstOfRHS);
        for (int t = 0; t < NUM_TOKENS; t++) {
            if (t == TK_COMMENT || t == TK_ERR) continue;
            if (bitsetContains(&firstOfRHS, t)) {
                if (T->entries[lhs][t] == PARSE_TABLE_ERROR) T->entries[lhs][t] = r;
            }
        }
        if (bitsetContains(&firstOfRHS, EPSILON_BIT)) {
            for (int t = 0; t < NUM_TOKENS; t++) {
                if (t == TK_COMMENT || t == TK_ERR) continue;
                if (bitsetContains(&F->followSet[lhs], t)) {
                    if (T->entries[lhs][t] == PARSE_TABLE_ERROR) T->entries[lhs][t] = r;
                }
            }
        }
    }
    for (int i = 0; i < NUM_NONTERMINALS; i++) {
        for (int t = 0; t < NUM_TOKENS; t++) {
            if (T->entries[i][t] == PARSE_TABLE_ERROR && bitsetContains(&F->followSet[i], t))
                T->entries[i][t] = PARSE_TABLE_SYNCH;
        }
    }
}

static TreeNode *createLeafNode(tokenInfo *tk) {
    TreeNode *node = (TreeNode *)calloc(1, sizeof(TreeNode));
    node->isLeaf = 1;
    strncpy(node->lexeme, tk->lexeme, MAX_LEXEME_LEN - 1);
    node->token = tk->token;
    node->lineNo = tk->lineNo;
    node->isNumber = tk->isNumber;
    if (tk->isNumber == 1) node->value.intVal = tk->value.intVal;
    else if (tk->isNumber == 2) node->value.realVal = tk->value.realVal;
    return node;
}

static TreeNode *createInternalNode(NonTerminal nt) {
    TreeNode *node = (TreeNode *)calloc(1, sizeof(TreeNode));
    node->isLeaf = 0;
    node->nonTerminal = nt;
    strcpy(node->lexeme, "----");
    node->token = TK_ERR;
    node->lineNo = -1;
    return node;
}

static void addChild(TreeNode *parent, TreeNode *child) {
    parent->numChildren++;
    parent->children = (TreeNode **)realloc(parent->children, parent->numChildren * sizeof(TreeNode *));
    parent->children[parent->numChildren - 1] = child;
    child->parent = parent;
}

#define MAX_STACK_SIZE 1000
typedef struct { int isTerminal; int id; TreeNode *treeNode; } StackEntry;
typedef struct { StackEntry items[MAX_STACK_SIZE]; int top; } ParseStack;
static void stackInit(ParseStack *s) { s->top = -1; }
static int stackIsEmpty(ParseStack *s) { return s->top == -1; }
static void stackPush(ParseStack *s, StackEntry e) {
    if (s->top < MAX_STACK_SIZE - 1) s->items[++(s->top)] = e;
}
static StackEntry stackPop(ParseStack *s) {
    if (s->top < 0) { StackEntry e = {1, TK_EOF, NULL}; return e; }
    return s->items[(s->top)--];
}
static StackEntry stackTop(ParseStack *s) {
    if (s->top < 0) { StackEntry e = {1, TK_EOF, NULL}; return e; }
    return s->items[s->top];
}

ParseTree parseInputSourceCode(char *testcaseFile, ParseTable *T, Grammar *G) {
    ParseTree PT; PT.root = NULL; int hasError = 0; int lastMatchedLine = 1;
    initLexer(testcaseFile);
    TreeNode *root = createInternalNode(NT_program); PT.root = root;
    ParseStack stack; stackInit(&stack);
    StackEntry dollarEntry = {1, TK_EOF, NULL}; stackPush(&stack, dollarEntry);
    StackEntry startEntry = {0, NT_program, root}; stackPush(&stack, startEntry);
    tokenInfo currentToken = getNextToken();
    while (currentToken.token == TK_ERR || currentToken.token == TK_COMMENT) {
        if (currentToken.token == TK_ERR) hasError = 1;
        currentToken = getNextToken();
    }
    while (!stackIsEmpty(&stack)) {
        StackEntry top = stackTop(&stack);
        if (top.isTerminal) {
            if (top.id == (int)currentToken.token) {
                stackPop(&stack);
                if (top.treeNode != NULL) {
                    strncpy(top.treeNode->lexeme, currentToken.lexeme, MAX_LEXEME_LEN - 1);
                    top.treeNode->token = currentToken.token;
                    top.treeNode->lineNo = currentToken.lineNo;
                    top.treeNode->isNumber = currentToken.isNumber;
                    if (currentToken.isNumber == 1) top.treeNode->value.intVal = currentToken.value.intVal;
                    else if (currentToken.isNumber == 2) top.treeNode->value.realVal = currentToken.value.realVal;
                }
                lastMatchedLine = currentToken.lineNo;
                if (currentToken.token == TK_EOF) break;
                currentToken = getNextToken();
                while (currentToken.token == TK_ERR || currentToken.token == TK_COMMENT) {
                    if (currentToken.token == TK_ERR) hasError = 1;
                    currentToken = getNextToken();
                }
            } else {
                hasError = 1;
                int errLine = (currentToken.lineNo != lastMatchedLine) ? lastMatchedLine : currentToken.lineNo;
                fprintf(stderr, "Line %d Error: The token %s for lexeme %s  does not match with the expected token %s\n",
                        errLine, getTokenName(currentToken.token), currentToken.lexeme, getTokenName((TokenType)top.id));
                stackPop(&stack);
            }
        } else {
            NonTerminal nt = (NonTerminal)top.id; TokenType a = currentToken.token;
            if (a == TK_EOF && T->entries[nt][(int)a] == PARSE_TABLE_ERROR) break;
            int ruleIndex = T->entries[nt][(int)a];
            if (ruleIndex == PARSE_TABLE_ERROR) {
                int hasEpsilon = 0;
                for (int ri = 0; ri < G->numRules; ri++) {
                    if (G->rules[ri].lhs == nt && G->rules[ri].rhsLen == 1 &&
                        G->rules[ri].rhs[0].isTerminal && G->rules[ri].rhs[0].id == EPSILON_SYM_ID) {
                        hasEpsilon = 1; break;
                    }
                }
                if (hasEpsilon) { stackPop(&stack); continue; }
                hasError = 1;
                fprintf(stderr, "Line %d Error: Invalid token %s encountered with value %s stack top %s\n",
                        currentToken.lineNo, getTokenName(currentToken.token), currentToken.lexeme, getNonTerminalName(nt));
                currentToken = getNextToken();
                while (currentToken.token == TK_ERR || currentToken.token == TK_COMMENT) {
                    if (currentToken.token == TK_ERR) hasError = 1;
                    currentToken = getNextToken();
                }
            } else if (ruleIndex == PARSE_TABLE_SYNCH) {
                hasError = 1;
                fprintf(stderr, "Line %d Error: Invalid token %s encountered with value %s stack top %s\n",
                        currentToken.lineNo, getTokenName(currentToken.token), currentToken.lexeme, getNonTerminalName(nt));
                stackPop(&stack);
            } else {
                stackPop(&stack); GrammarRule *rule = &G->rules[ruleIndex];
                if (top.treeNode) top.treeNode->ruleIndex = ruleIndex;
                if (rule->rhsLen == 1 && rule->rhs[0].isTerminal && rule->rhs[0].id == EPSILON_SYM_ID) continue;
                TreeNode **childNodes = (TreeNode **)malloc(rule->rhsLen * sizeof(TreeNode *));
                for (int i = 0; i < rule->rhsLen; i++) {
                    if (rule->rhs[i].isTerminal) {
                        tokenInfo dummy; memset(&dummy, 0, sizeof(tokenInfo));
                        dummy.token = (TokenType)rule->rhs[i].id;
                        childNodes[i] = createLeafNode(&dummy);
                    } else childNodes[i] = createInternalNode((NonTerminal)rule->rhs[i].id);
                    if (top.treeNode) addChild(top.treeNode, childNodes[i]);
                }
                for (int i = rule->rhsLen - 1; i >= 0; i--) {
                    StackEntry entry; entry.isTerminal = rule->rhs[i].isTerminal;
                    entry.id = rule->rhs[i].id; entry.treeNode = childNodes[i];
                    stackPush(&stack, entry);
                }
                free(childNodes);
            }
        }
    }
    cleanupLexer();
    if (!hasError) printf("Input source code is syntactically correct...........\n");
    return PT;
}

static void printNodeInfo(TreeNode *node, FILE *fout) {
    if (node == NULL) return;
    char lexemeStr[MAX_LEXEME_LEN];
    if (node->isLeaf) { strncpy(lexemeStr, node->lexeme, MAX_LEXEME_LEN - 1); lexemeStr[MAX_LEXEME_LEN - 1] = '\0'; }
    else strcpy(lexemeStr, "----");
    int lineNo = node->lineNo;
    const char *tokenName = node->isLeaf ? getTokenName(node->token) : "----";
    char valueStr[64];
    if (node->isNumber == 1) sprintf(valueStr, "%d", node->value.intVal);
    else if (node->isNumber == 2) sprintf(valueStr, "%lf", node->value.realVal);
    else strcpy(valueStr, "----");
    const char *parentStr = (node->parent == NULL) ? "ROOT" : (node->parent->isLeaf ? getTokenName(node->parent->token) : getNonTerminalName(node->parent->nonTerminal));
    const char *leafStr = node->isLeaf ? "yes" : "no";
    const char *nodeSymStr = node->isLeaf ? "----" : getNonTerminalName(node->nonTerminal);
    fprintf(fout, "%-25s %-5d %-20s %-20s %-30s %-5s %-30s\n",
            lexemeStr, lineNo, tokenName, valueStr, parentStr, leafStr, nodeSymStr);
}

static void printParseTreeInorder(TreeNode *node, FILE *fout) {
    if (node == NULL) return;
    if (node->isLeaf) { printNodeInfo(node, fout); return; }
    if (node->numChildren == 0) { printNodeInfo(node, fout); return; }
    printParseTreeInorder(node->children[0], fout);
    printNodeInfo(node, fout);
    for (int i = 1; i < node->numChildren; i++) printParseTreeInorder(node->children[i], fout);
}

void printParseTree(ParseTree *PT, char *outfile) {
    if (PT == NULL || PT->root == NULL) return;
    FILE *fout = fopen(outfile, "w");
    if (fout == NULL) return;
    fprintf(fout, "%-25s %-5s %-20s %-20s %-30s %-5s %-30s\n", "lexeme", "line", "tokenName", "valueIfNumber", "parentNodeSymbol", "isLeaf", "NodeSymbol");
    fprintf(fout, "%-25s %-5s %-20s %-20s %-30s %-5s %-30s\n", "-------", "----", "---------", "-------------", "----------------", "------", "----------");
    printParseTreeInorder(PT->root, fout);
    fclose(fout);
}

void freeParseTree(TreeNode *node) {
    if (node == NULL) return;
    for (int i = 0; i < node->numChildren; i++) freeParseTree(node->children[i]);
    free(node->children);
    free(node);
}
