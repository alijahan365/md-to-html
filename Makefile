.SUFFIXES:

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
RM = del /Q /F

TARGET = md2html.exe
OBJS = main.o parser.o semantic.o symbol_table.o html_generator.o utils.o lexer.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c -o utils.o

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c -o lexer.o

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o

semantic.o: semantic.c semantic.h
	$(CC) $(CFLAGS) -c semantic.c -o semantic.o

symbol_table.o: symbol_table.c symbol_table.h
	$(CC) $(CFLAGS) -c symbol_table.c -o symbol_table.o

html_generator.o: html_generator.c html_generator.h
	$(CC) $(CFLAGS) -c html_generator.c -o html_generator.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

# Flex & Bison target (Lab Syllabus exact commands)
flex-bison:
	bison -d parser.y
	flex lexer.l
	gcc parser.tab.c lex.yy.c -o flex_bison_md2html.exe

clean:
	-$(RM) *.o $(TARGET) flex_bison_md2html.exe lex.yy.c parser.tab.c parser.tab.h 2>NUL
