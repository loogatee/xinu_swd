#
# Make the Xinu configuration program
#
all:		config.o

config.o:		lex.yy.c y.tab.c
		cc -o config.o y.tab.c -ll

lex.yy.c:	config.l
		lex config.l

y.tab.c:	config.y
		yacc config.y

clean:
		rm -f lex.yy.c y.tab.c core config.o
