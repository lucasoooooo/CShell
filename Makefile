#By Lucas Balangero
#5/5/2020
CFLAGS = -ansi -Wall -g -O0 -Werror -Wshadow -Wwrite-strings \
-pedantic-errors -fstack-protector-all

.PHONY: all clean

all: d8sh
d8sh: d8sh.o executor.o parser.tab.o lexer.o
	gcc -lreadline d8sh.o executor.o parser.tab.o lexer.o -o d8sh
d8sh.o: d8sh.c executor.h lexer.h
	gcc $(CFLAGS) -c d8sh.c
executor.o: executor.c executor.h command.h
	gcc $(CFLAGS) -c executor.c
parser.tab.o: parser.tab.c command.h
	gcc $(CFLAGS) -c parser.tab.c
lexer.o: lexer.c
	gcc $(CFLAGS) -c lexer.c
	
clean:
	rm -f d8sh ./*~ ./*.o