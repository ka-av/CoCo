// Group number 34
// ID: 2023A7PS0564P 		Name: Soham Roy
//ID: 2023A7PS0678P 		Name: Kaavya Mer 
//ID: 2023A7PS0614P 		Name: Arindam Verma 
//ID: 2023A7PS0502P 		Name: Mona Gupta
//ID: 2023A7PS0509P 		Name: Tanushree Tokas
//ID: 2023A7PS0516P 		Name: Disha Goel

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "lexerDef.h"

FILE *getStream(FILE *fp);
void initLexer(const char *filename);
void cleanupLexer(void);
tokenInfo getNextToken(void);
const char *getTokenName(TokenType t);
void resetLexer(const char *filename);
void removeComments(const char *testcaseFile, const char *cleanFile);

#endif
