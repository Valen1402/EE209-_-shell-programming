# makefile
# Full name: Minh Quang Nguyen
# Student ID: 20190723

all:
	gcc209 -g lex.c buildin.c processhandler.c ish.c dynarray.c -o ish
clean:
	rm ish
