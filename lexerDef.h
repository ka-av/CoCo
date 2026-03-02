// Group number 34
// ID: 2023A7PS0564P 		Name: Soham Roy
//ID: 2023A7PS0678P 		Name: Kaavya Mer 
//ID: 2023A7PS0614P 		Name: Arindam Verma 
//ID: 2023A7PS0502P 		Name: Mona Gupta
//ID: 2023A7PS0509P 		Name: Tanushree Tokas
//ID: 2023A7PS0516P 		Name: Disha Goel

#ifndef LEXER_DEF_H
#define LEXER_DEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE       50
#define MAX_LEXEME_LEN    64
#define MAX_ID_LEN        20
#define MAX_FUNID_LEN     30
#define NUM_KEYWORDS      28

typedef enum {
    TK_ASSIGNOP,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,

    TK_LT,
    TK_LE,
    TK_GT,
    TK_GE,
    TK_EQ,
    TK_NE,

    TK_AND,
    TK_OR,
    TK_NOT,

    TK_SQL,
    TK_SQR,
    TK_OP,
    TK_CL,
    TK_COMMA,
    TK_SEM,
    TK_COLON,
    TK_DOT,

    TK_ID,
    TK_FUNID,
    TK_RUID,
    TK_FIELDID,

    TK_NUM,
    TK_RNUM,

    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_UNION,
    TK_ENDUNION,
    TK_DEFINETYPE,
    TK_AS,
    TK_TYPE,
    TK_MAIN,
    TK_GLOBAL,
    TK_PARAMETER,
    TK_LIST,
    TK_INPUT,
    TK_OUTPUT,
    TK_INT,
    TK_REAL,
    TK_ENDWHILE,
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,

    TK_COMMENT,
    TK_EOF,
    TK_ERR,

    NUM_TOKENS
} TokenType;

typedef struct {
    char      lexeme[MAX_LEXEME_LEN];
    TokenType token;
    int       lineNo;
    union {
        int    intVal;
        double realVal;
    } value;
    int isNumber;
} tokenInfo;

typedef struct {
    char   buf[2][BUFFER_SIZE + 1];
    int    activeBuf;
    int    pos;
    int    lineNo;
    FILE  *fp;
    int    bytesRead[2];
    int    eofReached;
    int    lexemeBeginBuf;
    int    lexemeBeginPos;
} TwinBuffer;

typedef struct {
    const char *keyword;
    TokenType   token;
} KeywordEntry;

#endif
