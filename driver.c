// Group number 34
// ID: 2023A7PS0564P 		Name: Soham Roy
//ID: 2023A7PS0678P 		Name: Kaavya Mer 
//ID: 2023A7PS0614P 		Name: Arindam Verma 
//ID: 2023A7PS0502P 		Name: Mona Gupta
//ID: 2023A7PS0509P 		Name: Tanushree Tokas
//ID: 2023A7PS0516P 		Name: Disha Goel

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: ./stage1exe testcase.txt parsetreeOutFile.txt\n");
        return 1;
    }

    char *inputFile = argv[1];
    char *outputFile = argv[2];
    int choice;

    printf("(c) Both lexical and syntax analysis modules implemented\n");

    while (1) {
        printf("\nSelect an option:\n");
        printf("0: Exit\n");
        printf("1: Remove comments and print to console\n");
        printf("2: Print token list (Lexical Analysis)\n");
        printf("3: Verify syntactic correctness and print Parse Tree\n");
        printf("4: Total time taken by Lexer and Parser\n");
        printf("Choice: ");
        
        if (scanf("%d", &choice) != 1) break;
        if (choice == 0) break;

        switch (choice) {
            case 1:
                removeComments(inputFile, NULL);
                break;

            case 2: {
                initLexer(inputFile);
                tokenInfo tk;
                while ((tk = getNextToken()).token != TK_EOF) {
                    if (tk.token != TK_ERR) {
                        printf("Line %d: %s (Token: %s)\n", tk.lineNo, tk.lexeme, getTokenName(tk.token));
                    }
                }
                cleanupLexer();
                break;
            }

            case 3: {
                Grammar G;
                initGrammar(&G);
                FirstAndFollow F = ComputeFirstAndFollowSets(&G);
                ParseTable T;
                createParseTable(&F, &T, &G);
                
                ParseTree PT = parseInputSourceCode(inputFile, &T, &G);
                if (PT.root != NULL) {
                    printParseTree(&PT, outputFile);
                    freeParseTree(PT.root);
                }
                break;
            }

            case 4: {
                clock_t start_time, end_time;
                double total_CPU_time, total_CPU_time_in_seconds;
                
                start_time = clock();
                
                Grammar G;
                initGrammar(&G);
                FirstAndFollow F = ComputeFirstAndFollowSets(&G);
                ParseTable T;
                createParseTable(&F, &T, &G);
                ParseTree PT = parseInputSourceCode(inputFile, &T, &G);
                
                end_time = clock();
                
                if (PT.root != NULL) freeParseTree(PT.root);
                
                total_CPU_time = (double)(end_time - start_time);
                total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
                
                printf("Total CPU time: %f ticks\n", total_CPU_time);
                printf("Total CPU time in seconds: %f sec\n", total_CPU_time_in_seconds);
                break;
            }

            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}