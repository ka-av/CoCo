// Group number 34
// ID: 2023A7PS0564P 		Name: Soham Roy
//ID: 2023A7PS0678P 		Name: Kaavya Mer 
//ID: 2023A7PS0614P 		Name: Arindam Verma 
//ID: 2023A7PS0502P 		Name: Mona Gupta
//ID: 2023A7PS0509P 		Name: Tanushree Tokas
//ID: 2023A7PS0516P 		Name: Disha Goel

#include "lexerDef.h"

static const char *tokenNames[] = {
    "TK_ASSIGNOP", "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV",
    "TK_LT", "TK_LE", "TK_GT", "TK_GE", "TK_EQ", "TK_NE",
    "TK_AND", "TK_OR", "TK_NOT",
    "TK_SQL", "TK_SQR", "TK_OP", "TK_CL",
    "TK_COMMA", "TK_SEM", "TK_COLON", "TK_DOT",
    "TK_ID", "TK_FUNID", "TK_RUID", "TK_FIELDID",
    "TK_NUM", "TK_RNUM",
    "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE", "TK_UNION",
    "TK_ENDUNION", "TK_DEFINETYPE", "TK_AS", "TK_TYPE", "TK_MAIN",
    "TK_GLOBAL", "TK_PARAMETER", "TK_LIST", "TK_INPUT", "TK_OUTPUT",
    "TK_INT", "TK_REAL", "TK_ENDWHILE", "TK_IF", "TK_THEN",
    "TK_ENDIF", "TK_READ", "TK_WRITE", "TK_RETURN", "TK_CALL",
    "TK_RECORD", "TK_ENDRECORD", "TK_ELSE",
    "TK_COMMENT", "TK_EOF", "TK_ERR"
};

const char *getTokenName(TokenType t) {
    if (t >= 0 && t < NUM_TOKENS)
        return tokenNames[t];
    return "UNKNOWN";
}

static KeywordEntry keywordTable[] = {
    {"with",       TK_WITH},
    {"parameters", TK_PARAMETERS},
    {"end",        TK_END},
    {"while",      TK_WHILE},
    {"union",      TK_UNION},
    {"endunion",   TK_ENDUNION},
    {"definetype", TK_DEFINETYPE},
    {"as",         TK_AS},
    {"type",       TK_TYPE},
    {"_main",      TK_MAIN},
    {"global",     TK_GLOBAL},
    {"parameter",  TK_PARAMETER},
    {"list",       TK_LIST},
    {"input",      TK_INPUT},
    {"output",     TK_OUTPUT},
    {"int",        TK_INT},
    {"real",       TK_REAL},
    {"endwhile",   TK_ENDWHILE},
    {"if",         TK_IF},
    {"then",       TK_THEN},
    {"endif",      TK_ENDIF},
    {"read",       TK_READ},
    {"write",      TK_WRITE},
    {"return",     TK_RETURN},
    {"call",       TK_CALL},
    {"record",     TK_RECORD},
    {"endrecord",  TK_ENDRECORD},
    {"else",       TK_ELSE},
    {NULL,         TK_ERR}
};

static TokenType lookupKeyword(const char *lexeme) {
    for (int i = 0; keywordTable[i].keyword != NULL; i++) {
        if (strcmp(keywordTable[i].keyword, lexeme) == 0)
            return keywordTable[i].token;
    }
    return TK_ERR;
}

static TwinBuffer twinBuf;

static void fillBuffer(int bufIdx) {
    if (twinBuf.fp == NULL) {
        twinBuf.bytesRead[bufIdx] = 0;
        return;
    }
    int n = (int)fread(twinBuf.buf[bufIdx], 1, BUFFER_SIZE, twinBuf.fp);
    twinBuf.bytesRead[bufIdx] = n;
    twinBuf.buf[bufIdx][n] = '\0';
    if (n < BUFFER_SIZE) {
        twinBuf.eofReached = 1;
    }
}

FILE *getStream(FILE *fp) {
    twinBuf.fp = fp;
    if (fp == NULL) return NULL;
    fillBuffer(0);
    twinBuf.activeBuf = 0;
    twinBuf.pos = 0;
    twinBuf.bytesRead[1] = 0;
    return fp;
}

void initLexer(const char *filename) {
    memset(&twinBuf, 0, sizeof(TwinBuffer));
    twinBuf.lineNo = 1;
    twinBuf.eofReached = 0;
    twinBuf.fp = fopen(filename, "r");
    if (twinBuf.fp == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return;
    }
    fillBuffer(0);
    twinBuf.activeBuf = 0;
    twinBuf.pos = 0;
    twinBuf.lexemeBeginBuf = 0;
    twinBuf.lexemeBeginPos = 0;
}

void cleanupLexer(void) {
    if (twinBuf.fp != NULL) {
        fclose(twinBuf.fp);
        twinBuf.fp = NULL;
    }
}

void resetLexer(const char *filename) {
    cleanupLexer();
    initLexer(filename);
}

static int getNextChar(void) {
    if (twinBuf.pos >= twinBuf.bytesRead[twinBuf.activeBuf]) {
        if (twinBuf.eofReached && twinBuf.pos >= twinBuf.bytesRead[twinBuf.activeBuf]) {
            int otherBuf = 1 - twinBuf.activeBuf;
            if (twinBuf.bytesRead[otherBuf] > 0) {
            }
            if (twinBuf.eofReached) {
                return EOF;
            }
        }
        int nextBuf = 1 - twinBuf.activeBuf;
        if (!twinBuf.eofReached) {
            fillBuffer(nextBuf);
        } else {
            twinBuf.bytesRead[nextBuf] = 0;
        }
        twinBuf.activeBuf = nextBuf;
        twinBuf.pos = 0;
        if (twinBuf.bytesRead[twinBuf.activeBuf] == 0) {
            return EOF;
        }
    }
    char c = twinBuf.buf[twinBuf.activeBuf][twinBuf.pos];
    twinBuf.pos++;
    return (unsigned char)c;
}

static void retract(int count) {
    for (int i = 0; i < count; i++) {
        if (twinBuf.pos > 0) {
            twinBuf.pos--;
        } else {
            int prevBuf = 1 - twinBuf.activeBuf;
            if (twinBuf.bytesRead[prevBuf] > 0) {
                twinBuf.activeBuf = prevBuf;
                twinBuf.pos = twinBuf.bytesRead[prevBuf] - 1;
            }
        }
    }
}

static void markLexemeBegin(void) {
    twinBuf.lexemeBeginBuf = twinBuf.activeBuf;
    twinBuf.lexemeBeginPos = twinBuf.pos;
}

static tokenInfo makeToken(TokenType type, const char *lexeme, int lineNo) {
    tokenInfo tk;
    memset(&tk, 0, sizeof(tokenInfo));
    tk.token = type;
    tk.lineNo = lineNo;
    tk.isNumber = 0;
    if (lexeme != NULL) {
        strncpy(tk.lexeme, lexeme, MAX_LEXEME_LEN - 1);
        tk.lexeme[MAX_LEXEME_LEN - 1] = '\0';
    } else {
        tk.lexeme[0] = '\0';
    }
    return tk;
}

static tokenInfo makeErrorToken(const char *lexeme, int lineNo) {
    tokenInfo tk = makeToken(TK_ERR, lexeme, lineNo);
    return tk;
}

tokenInfo getNextToken(void) {
    int c;
    char lexeme[MAX_LEXEME_LEN];
    int lexLen = 0;
    int startLine;

    while (1) {
        c = getNextChar();
        if (c == EOF) {
            return makeToken(TK_EOF, "$", twinBuf.lineNo);
        }

        if (c == ' ' || c == '\t' || c == '\r') {
            continue;
        }
        if (c == '\n') {
            twinBuf.lineNo++;
            continue;
        }

        if (c == '%') {
            int commentLine = twinBuf.lineNo;
            while (1) {
                c = getNextChar();
                if (c == '\n') {
                    twinBuf.lineNo++;
                    break;
                }
                if (c == EOF) break;
            }
            return makeToken(TK_COMMENT, "%", commentLine);
        }

        break;
    }

    startLine = twinBuf.lineNo;
    markLexemeBegin();
    lexLen = 0;
    memset(lexeme, 0, sizeof(lexeme));

    if (c == '<') {
        lexeme[lexLen++] = (char)c;
        c = getNextChar();
        if (c == '-') {
            lexeme[lexLen++] = (char)c;
            c = getNextChar();
            if (c == '-') {
                lexeme[lexLen++] = (char)c;
                c = getNextChar();
                if (c == '-') {
                    lexeme[lexLen++] = (char)c;
                    lexeme[lexLen] = '\0';
                    return makeToken(TK_ASSIGNOP, lexeme, startLine);
                } else {
                    lexeme[lexLen] = '\0';
                    if (c != EOF) retract(1);
                    fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
                    return makeErrorToken(lexeme, startLine);
                }
            } else {
                if (c != EOF) retract(1);
                retract(1);
                lexeme[0] = '<'; lexeme[1] = '\0';
                return makeToken(TK_LT, lexeme, startLine);
            }
        } else if (c == '=') {
            lexeme[lexLen++] = (char)c;
            lexeme[lexLen] = '\0';
            return makeToken(TK_LE, lexeme, startLine);
        } else {
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';
            return makeToken(TK_LT, lexeme, startLine);
        }
    }

    if (c == '>') {
        lexeme[lexLen++] = (char)c;
        c = getNextChar();
        if (c == '=') {
            lexeme[lexLen++] = (char)c;
            lexeme[lexLen] = '\0';
            return makeToken(TK_GE, lexeme, startLine);
        } else {
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';
            return makeToken(TK_GT, lexeme, startLine);
        }
    }

    if (c == '=') {
        lexeme[lexLen++] = (char)c;
        c = getNextChar();
        if (c == '=') {
            lexeme[lexLen++] = (char)c;
            lexeme[lexLen] = '\0';
            return makeToken(TK_EQ, lexeme, startLine);
        } else {
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';
            fprintf(stderr, "Line no. %d Error: Unknown Symbol <%s>\n", startLine, lexeme);
            return makeErrorToken(lexeme, startLine);
        }
    }

    if (c == '!') {
        lexeme[lexLen++] = (char)c;
        c = getNextChar();
        if (c == '=') {
            lexeme[lexLen++] = (char)c;
            lexeme[lexLen] = '\0';
            return makeToken(TK_NE, lexeme, startLine);
        } else {
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';
            fprintf(stderr, "Line no. %d Error: Unknown Symbol <%s>\n", startLine, lexeme);
            return makeErrorToken(lexeme, startLine);
        }
    }

    if (c == '&') {
        lexeme[lexLen++] = (char)c;
        c = getNextChar();
        if (c == '&') {
            lexeme[lexLen++] = (char)c;
            c = getNextChar();
            if (c == '&') {
                lexeme[lexLen++] = (char)c;
                lexeme[lexLen] = '\0';
                return makeToken(TK_AND, lexeme, startLine);
            } else {
                if (c != EOF) retract(1);
                lexeme[lexLen] = '\0';
                fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
                return makeErrorToken(lexeme, startLine);
            }
        } else {
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';
            fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
            return makeErrorToken(lexeme, startLine);
        }
    }

    if (c == '@') {
        lexeme[lexLen++] = (char)c;
        c = getNextChar();
        if (c == '@') {
            lexeme[lexLen++] = (char)c;
            c = getNextChar();
            if (c == '@') {
                lexeme[lexLen++] = (char)c;
                lexeme[lexLen] = '\0';
                return makeToken(TK_OR, lexeme, startLine);
            } else {
                if (c != EOF) retract(1);
                lexeme[lexLen] = '\0';
                fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
                return makeErrorToken(lexeme, startLine);
            }
        } else {
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';
            fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
            return makeErrorToken(lexeme, startLine);
        }
    }

    if (c == '~') { lexeme[0] = '~'; lexeme[1] = '\0'; return makeToken(TK_NOT, lexeme, startLine); }
    if (c == '+') { lexeme[0] = '+'; lexeme[1] = '\0'; return makeToken(TK_PLUS, lexeme, startLine); }
    if (c == '-') { lexeme[0] = '-'; lexeme[1] = '\0'; return makeToken(TK_MINUS, lexeme, startLine); }
    if (c == '*') { lexeme[0] = '*'; lexeme[1] = '\0'; return makeToken(TK_MUL, lexeme, startLine); }
    if (c == '/') { lexeme[0] = '/'; lexeme[1] = '\0'; return makeToken(TK_DIV, lexeme, startLine); }
    if (c == '(') { lexeme[0] = '('; lexeme[1] = '\0'; return makeToken(TK_OP, lexeme, startLine); }
    if (c == ')') { lexeme[0] = ')'; lexeme[1] = '\0'; return makeToken(TK_CL, lexeme, startLine); }
    if (c == '[') { lexeme[0] = '['; lexeme[1] = '\0'; return makeToken(TK_SQL, lexeme, startLine); }
    if (c == ']') { lexeme[0] = ']'; lexeme[1] = '\0'; return makeToken(TK_SQR, lexeme, startLine); }
    if (c == ',') { lexeme[0] = ','; lexeme[1] = '\0'; return makeToken(TK_COMMA, lexeme, startLine); }
    if (c == ';') { lexeme[0] = ';'; lexeme[1] = '\0'; return makeToken(TK_SEM, lexeme, startLine); }
    if (c == ':') { lexeme[0] = ':'; lexeme[1] = '\0'; return makeToken(TK_COLON, lexeme, startLine); }
    if (c == '.') { lexeme[0] = '.'; lexeme[1] = '\0'; return makeToken(TK_DOT, lexeme, startLine); }

    if (isdigit(c)) {
        lexeme[lexLen++] = (char)c;
        while (1) {
            c = getNextChar();
            if (c != EOF && isdigit(c)) {
                if (lexLen < MAX_LEXEME_LEN - 1)
                    lexeme[lexLen++] = (char)c;
            } else {
                break;
            }
        }

        if (c == '.') {
            int d1 = getNextChar();
            if (d1 != EOF && isdigit(d1)) {
                int d2 = getNextChar();
                if (d2 != EOF && isdigit(d2)) {
                    lexeme[lexLen++] = '.';
                    lexeme[lexLen++] = (char)d1;
                    lexeme[lexLen++] = (char)d2;

                    int e = getNextChar();
                    if (e == 'E') {
                        int sign = getNextChar();
                        int e1, e2;
                        if (sign == '+' || sign == '-') {
                            e1 = getNextChar();
                            e2 = getNextChar();
                            if (e1 != EOF && isdigit(e1) && e2 != EOF && isdigit(e2)) {
                                lexeme[lexLen++] = 'E';
                                lexeme[lexLen++] = (char)sign;
                                lexeme[lexLen++] = (char)e1;
                                lexeme[lexLen++] = (char)e2;
                                lexeme[lexLen] = '\0';
                                tokenInfo tk = makeToken(TK_RNUM, lexeme, startLine);
                                tk.isNumber = 2;
                                tk.value.realVal = atof(lexeme);
                                return tk;
                            } else {
                                lexeme[lexLen++] = 'E';
                                lexeme[lexLen++] = (char)sign;
                                if (e1 != EOF) lexeme[lexLen++] = (char)e1;
                                if (e2 != EOF) lexeme[lexLen++] = (char)e2;
                                lexeme[lexLen] = '\0';
                                if (e2 != EOF && !isdigit(e2)) retract(1);
                                if (e1 != EOF && !isdigit(e1)) retract(1);
                                fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
                                return makeErrorToken(lexeme, startLine);
                            }
                        } else if (sign != EOF && isdigit(sign)) {
                            e2 = getNextChar();
                            if (e2 != EOF && isdigit(e2)) {
                                lexeme[lexLen++] = 'E';
                                lexeme[lexLen++] = (char)sign;
                                lexeme[lexLen++] = (char)e2;
                                lexeme[lexLen] = '\0';
                                tokenInfo tk = makeToken(TK_RNUM, lexeme, startLine);
                                tk.isNumber = 2;
                                tk.value.realVal = atof(lexeme);
                                return tk;
                            } else {
                                lexeme[lexLen++] = 'E';
                                lexeme[lexLen++] = (char)sign;
                                if (e2 != EOF) lexeme[lexLen++] = (char)e2;
                                lexeme[lexLen] = '\0';
                                if (e2 != EOF && !isdigit(e2)) retract(1);
                                fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
                                return makeErrorToken(lexeme, startLine);
                            }
                        } else {
                            lexeme[lexLen++] = 'E';
                            if (sign != EOF) lexeme[lexLen++] = (char)sign;
                            lexeme[lexLen] = '\0';
                            if (sign != EOF) retract(1);
                            fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
                            return makeErrorToken(lexeme, startLine);
                        }
                    } else {
                        if (e != EOF) retract(1);
                        lexeme[lexLen] = '\0';
                        tokenInfo tk = makeToken(TK_RNUM, lexeme, startLine);
                        tk.isNumber = 2;
                        tk.value.realVal = atof(lexeme);
                        return tk;
                    }
                } else {
                    lexeme[lexLen++] = '.';
                    lexeme[lexLen++] = (char)d1;
                    if (d2 != EOF && !isdigit(d2)) {
                        lexeme[lexLen] = '\0';
                        retract(1);
                    } else if (d2 == EOF) {
                        lexeme[lexLen] = '\0';
                    }
                    fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
                    return makeErrorToken(lexeme, startLine);
                }
            } else {
                if (d1 != EOF) retract(1);
                retract(1);
                lexeme[lexLen] = '\0';
                tokenInfo tk = makeToken(TK_NUM, lexeme, startLine);
                tk.isNumber = 1;
                tk.value.intVal = atoi(lexeme);
                return tk;
            }
        } else {
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';
            tokenInfo tk = makeToken(TK_NUM, lexeme, startLine);
            tk.isNumber = 1;
            tk.value.intVal = atoi(lexeme);
            return tk;
        }
    }

    if (c == '_') {
        lexeme[lexLen++] = (char)c;
        c = getNextChar();
        if (c != EOF && isalpha(c)) {
            lexeme[lexLen++] = (char)c;
            while (1) {
                c = getNextChar();
                if (c != EOF && isalpha(c)) {
                    if (lexLen < MAX_LEXEME_LEN - 1)
                        lexeme[lexLen++] = (char)c;
                } else {
                    break;
                }
            }
            while (c != EOF && isdigit(c)) {
                if (lexLen < MAX_LEXEME_LEN - 1)
                    lexeme[lexLen++] = (char)c;
                c = getNextChar();
            }
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';

            if (strcmp(lexeme, "_main") == 0) {
                return makeToken(TK_MAIN, lexeme, startLine);
            }

            if (lexLen > MAX_FUNID_LEN) {
                fprintf(stderr, "Line no. %d Error: Function Identifier is longer than the prescribed length of %d characters.\n",
                        startLine, MAX_FUNID_LEN);
                return makeErrorToken(lexeme, startLine);
            }

            return makeToken(TK_FUNID, lexeme, startLine);
        } else {
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';
            fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
            return makeErrorToken(lexeme, startLine);
        }
    }

    if (c == '#') {
        lexeme[lexLen++] = (char)c;
        c = getNextChar();
        if (c != EOF && c >= 'a' && c <= 'z') {
            lexeme[lexLen++] = (char)c;
            while (1) {
                c = getNextChar();
                if (c != EOF && c >= 'a' && c <= 'z') {
                    if (lexLen < MAX_LEXEME_LEN - 1)
                        lexeme[lexLen++] = (char)c;
                } else {
                    break;
                }
            }
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';
            return makeToken(TK_RUID, lexeme, startLine);
        } else {
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';
            fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
            return makeErrorToken(lexeme, startLine);
        }
    }

    if (c >= 'a' && c <= 'z') {
        lexeme[lexLen++] = (char)c;

        if (c >= 'b' && c <= 'd') {
            c = getNextChar();
            if (c != EOF && c >= '2' && c <= '7') {
                lexeme[lexLen++] = (char)c;

                int inLetterPart = 1;
                while (1) {
                    c = getNextChar();
                    if (c != EOF && c >= 'b' && c <= 'd' && inLetterPart) {
                        if (lexLen < MAX_LEXEME_LEN - 1)
                            lexeme[lexLen++] = (char)c;
                    } else if (c != EOF && c >= '2' && c <= '7') {
                        inLetterPart = 0;
                        if (lexLen < MAX_LEXEME_LEN - 1)
                            lexeme[lexLen++] = (char)c;
                    } else {
                        break;
                    }
                }
                if (c != EOF) retract(1);
                lexeme[lexLen] = '\0';

                if (lexLen > MAX_ID_LEN) {
                    fprintf(stderr, "Line no. %d Error: Variable Identifier is longer than the prescribed length of %d characters.\n",
                            startLine, MAX_ID_LEN);
                    return makeErrorToken(lexeme, startLine);
                }

                return makeToken(TK_ID, lexeme, startLine);
            } else {
                while (c != EOF && c >= 'a' && c <= 'z') {
                    if (lexLen < MAX_LEXEME_LEN - 1)
                        lexeme[lexLen++] = (char)c;
                    c = getNextChar();
                }
                if (c != EOF) retract(1);
                lexeme[lexLen] = '\0';

                TokenType kw = lookupKeyword(lexeme);
                if (kw != TK_ERR) {
                    return makeToken(kw, lexeme, startLine);
                }
                return makeToken(TK_FIELDID, lexeme, startLine);
            }
        } else {
            while (1) {
                c = getNextChar();
                if (c != EOF && c >= 'a' && c <= 'z') {
                    if (lexLen < MAX_LEXEME_LEN - 1)
                        lexeme[lexLen++] = (char)c;
                } else {
                    break;
                }
            }
            if (c != EOF) retract(1);
            lexeme[lexLen] = '\0';

            TokenType kw = lookupKeyword(lexeme);
            if (kw != TK_ERR) {
                return makeToken(kw, lexeme, startLine);
            }
            return makeToken(TK_FIELDID, lexeme, startLine);
        }
    }

    if (c >= 'A' && c <= 'Z') {
        lexeme[lexLen++] = (char)c;
        while (1) {
            c = getNextChar();
            if (c != EOF && isalpha(c)) {
                if (lexLen < MAX_LEXEME_LEN - 1)
                    lexeme[lexLen++] = (char)c;
            } else {
                break;
            }
        }
        if (c != EOF) retract(1);
        lexeme[lexLen] = '\0';
        fprintf(stderr, "Line no. %d Error: Unknown pattern <%s>\n", startLine, lexeme);
        return makeErrorToken(lexeme, startLine);
    }

    lexeme[0] = (char)c;
    lexeme[1] = '\0';
    fprintf(stderr, "Line no. %d Error: Unknown Symbol <%s>\n", startLine, lexeme);
    return makeErrorToken(lexeme, startLine);
}

void removeComments(const char *testcaseFile, const char *cleanFile) {
    FILE *fin = fopen(testcaseFile, "r");
    if (fin == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", testcaseFile);
        return;
    }
    FILE *fout = NULL;
    if (cleanFile != NULL) {
        fout = fopen(cleanFile, "w");
    }

    int ch;
    int inComment = 0;
    while ((ch = fgetc(fin)) != EOF) {
        if (inComment) {
            if (ch == '\n') {
                inComment = 0;
                printf("\n");
                if (fout) fputc('\n', fout);
            }
        } else {
            if (ch == '%') {
                inComment = 1;
            } else {
                printf("%c", ch);
                if (fout) fputc(ch, fout);
            }
        }
    }

    fclose(fin);
    if (fout) fclose(fout);
}
