CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99 -g
HEADERS=headers/proceso.h headers/planificador.h headers/printer.h headers/conf.h headers/cola.h headers/parser.h headers/dispositivo.h headers/semaforo.h
OBJ=dispositivo.o cola.o proceso.o planificador.o printer.o parser.o semaforo.o main.o 

EXEC=./main programs/.conf programs/proc1 programs/proc2 programs/proc3

all: build

build: $(OBJ) 
	$(CC) $(CFLAGS) -o main $(OBJ)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

test: build
	$(EXEC)

debug: build
	gdb --args $(EXEC)

check: build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s $(EXEC)

plan: test
	pdflatex -output-directory .. ../planificacion.tex 
	rm ../planificacion.aux ../planificacion.log
	xdg-open ../planificacion.pdf

clean:
	rm -f *.o main ../planificacion.pdf ../planificacion.tex ../planificacion.log ../planificacion.aux vgcore.*

.PHONY: all clean
