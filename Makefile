# Compilador e Flags
CC = gcc

#O '-I include' avisa o gcc para procurar os cabecalhos .h 
#na pasta include
# -D_POSIX_C_SOURCE=199309L: habilita o clock_gettime no padrão C99
CFLAGS = -Wall -Wextra -std=c99 -pedantic -I include -D_POSIX_C_SOURCE=199309L

.PHONY: all run clean

# Regra principal: gera o executavel 'trabalho'
all: trabalho

trabalho: main.o cli.o item.o reader_nasa.o
	$(CC) main.o cli.o item.o reader_nasa.o -o trabalho $(CFLAGS)

#Compilacao dos modulos individuais
main.o: apps/main.c include/cli.h include/item.h include/reader_nasa.h
	$(CC) -c apps/main.c -o main.o $(CFLAGS)

cli.o: source/cli.c include/cli.h 
	$(CC) -c source/cli.c -o cli.o $(CFLAGS)

item.o: source/item.c include/item.h
	$(CC) -c source/item.c -o item.o $(CFLAGS)

reader_nasa.o: source/reader_nasa.c include/reader_nasa.h
	$(CC) -c source/reader_nasa.c -o reader_nasa.o $(CFLAGS)

run: trabalho 
	./trabalho 

# Regra de limpeza:
clean:
	rm -f *.o trabalho