// Group number 34
// ID: 2023A7PS0564P 		Name: Soham Roy
//ID: 2023A7PS0678P 		Name: Kaavya Mer 
//ID: 2023A7PS0614P 		Name: Arindam Verma 
//ID: 2023A7PS0502P 		Name: Mona Gupta
//ID: 2023A7PS0509P 		Name: Tanushree Tokas
//ID: 2023A7PS0516P 		Name: Disha Goel

#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

void initGrammar(Grammar *G);
FirstAndFollow ComputeFirstAndFollowSets(Grammar *G);
void createParseTable(FirstAndFollow *F, ParseTable *T, Grammar *G);
ParseTree parseInputSourceCode(char *testcaseFile, ParseTable *T, Grammar *G);
void printParseTree(ParseTree *PT, char *outFile);
void freeParseTree(TreeNode *node);

#endif
