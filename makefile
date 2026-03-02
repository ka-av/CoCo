# Group number 34
# ID: 2023A7PS0564P 		Name: Soham Roy
# ID: 2023A7PS0678P 		Name: Kaavya Mer
# ID: 2023A7PS0614P 		Name: Arindam Verma
# ID: 2023A7PS0502P 		Name: Mona Gupta
# ID: 2023A7PS0509P 		Name: Tanushree Tokas
# ID: 2023A7PS0516P 		Name: Disha Goel

CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm

TARGET = stage1exe
OBJS = driver.o lexer.o parser.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

driver.o: driver.c lexer.h parser.h lexerDef.h parserDef.h
	$(CC) $(CFLAGS) -c driver.c

lexer.o: lexer.c lexerDef.h
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c parserDef.h lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
