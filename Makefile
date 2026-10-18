# compilador e flags
CC = gcc

# -i include indica onde estao os cabecalhos
# -d_posix_c_source habilita clock_gettime no c99
CFLAGS = -Wall -Wextra -std=c99 -pedantic -I include -D_POSIX_C_SOURCE=199309L

# objetos dos tads
TADS_OBJ = pilha.o fila.o lista_seq.o lista_enc.o lista_cabeca.o lista_ord.o lista_gen.o lista_cruz.o

.PHONY: all run clean teste

# gera o executavel principal
all: trabalho

trabalho: main.o cli.o item.o query.o estrutura.o reader_nasa.o $(TADS_OBJ)
	$(CC) main.o cli.o item.o query.o estrutura.o reader_nasa.o $(TADS_OBJ) -o trabalho $(CFLAGS)

# compila os modulos individuais
main.o: apps/main.c include/cli.h include/item.h include/query.h include/estrutura.h include/reader_nasa.h
	$(CC) -c apps/main.c -o main.o $(CFLAGS)

cli.o: source/cli.c include/cli.h 
	$(CC) -c source/cli.c -o cli.o $(CFLAGS)

item.o: source/item.c include/item.h
	$(CC) -c source/item.c -o item.o $(CFLAGS)

query.o: source/query.c include/query.h include/item.h
	$(CC) -c source/query.c -o query.o $(CFLAGS)

estrutura.o: source/estrutura.c include/estrutura.h include/query.h include/item.h
	$(CC) -c source/estrutura.c -o estrutura.o $(CFLAGS)

reader_nasa.o: source/reader_nasa.c include/reader_nasa.h include/estrutura.h include/item.h
	$(CC) -c source/reader_nasa.c -o reader_nasa.o $(CFLAGS)

# compila os tads
pilha.o: source/tads/pilha.c include/tads/pilha.h include/item.h
	$(CC) -c source/tads/pilha.c -o pilha.o $(CFLAGS)

fila.o: source/tads/fila.c include/tads/fila.h include/item.h
	$(CC) -c source/tads/fila.c -o fila.o $(CFLAGS)

lista_seq.o: source/tads/lista_seq.c include/tads/lista_seq.h include/item.h
	$(CC) -c source/tads/lista_seq.c -o lista_seq.o $(CFLAGS)

lista_enc.o: source/tads/lista_enc.c include/tads/lista_enc.h include/item.h
	$(CC) -c source/tads/lista_enc.c -o lista_enc.o $(CFLAGS)

lista_cabeca.o: source/tads/lista_cabeca.c include/tads/lista_cabeca.h include/item.h
	$(CC) -c source/tads/lista_cabeca.c -o lista_cabeca.o $(CFLAGS)

lista_ord.o: source/tads/lista_ord.c include/tads/lista_ord.h include/item.h
	$(CC) -c source/tads/lista_ord.c -o lista_ord.o $(CFLAGS)

lista_gen.o: source/tads/lista_gen.c include/tads/lista_gen.h include/item.h
	$(CC) -c source/tads/lista_gen.c -o lista_gen.o $(CFLAGS)

lista_cruz.o: source/tads/lista_cruz.c include/tads/lista_cruz.h include/item.h
	$(CC) -c source/tads/lista_cruz.c -o lista_cruz.o $(CFLAGS)

# executa um exemplo com a base inteira
run: trabalho
	./trabalho --reader nasa --input dados/asteroids.csv --base lista_enc --result lista_ord --query consultas/q1.txt --output resultado.csv --top-k 100 --limit 0

# executa os testes unitarios dos tads
teste:
	$(MAKE) -C tests run

# remove arquivos gerados
clean:
	rm -f *.o trabalho resultado.csv
	$(MAKE) -C tests clean
