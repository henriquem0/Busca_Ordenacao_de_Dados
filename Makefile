# compilador e flags
CC = gcc

# -i include indica onde estao os cabecalhos
# -d_posix_c_source habilita clock_gettime no c99
CFLAGS = -Wall -Wextra -std=c99 -pedantic -I include -D_POSIX_C_SOURCE=199309L

BUILD_DIR = build

TARGET = $(BUILD_DIR)/trabalho

# objetos dos tads
TADS_OBJ = $(addprefix $(BUILD_DIR)/, pilha.o fila.o lista_seq.o lista_enc.o lista_cabeca.o lista_ord.o lista_gen.o lista_cruz.o)

OBJ = $(BUILD_DIR)/main.o \
	  $(BUILD_DIR)/cli.o \
	  $(BUILD_DIR)/item.o \
	  $(BUILD_DIR)/query.o \
	  $(BUILD_DIR)/estrutura.o \
	  $(BUILD_DIR)/reader_antt.o \
	  $(TADS_OBJ)

.PHONY: all run clean teste

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# gera o executavel principal
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

# compila os modulos individuais
$(BUILD_DIR)/main.o: apps/main.c include/cli.h include/item.h include/query.h include/estrutura.h include/reader_antt.h | $(BUILD_DIR)
	$(CC) -c apps/main.c -o $(BUILD_DIR)/main.o $(CFLAGS)

$(BUILD_DIR)/cli.o: source/cli.c include/cli.h | $(BUILD_DIR)
	$(CC) -c source/cli.c -o $(BUILD_DIR)/cli.o $(CFLAGS)

$(BUILD_DIR)/item.o: source/item.c include/item.h | $(BUILD_DIR)
	$(CC) -c source/item.c -o $(BUILD_DIR)/item.o $(CFLAGS)

$(BUILD_DIR)/query.o: source/query.c include/query.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/query.c -o $(BUILD_DIR)/query.o $(CFLAGS)

$(BUILD_DIR)/estrutura.o: source/estrutura.c include/estrutura.h include/query.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/estrutura.c -o $(BUILD_DIR)/estrutura.o $(CFLAGS)

$(BUILD_DIR)/reader_antt.o: source/reader_antt.c include/reader_antt.h include/estrutura.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/reader_antt.c -o $(BUILD_DIR)/reader_antt.o $(CFLAGS)

# compila os tads
$(BUILD_DIR)/pilha.o: source/tads/pilha.c include/tads/pilha.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/tads/pilha.c -o $(BUILD_DIR)/pilha.o $(CFLAGS)

$(BUILD_DIR)/fila.o: source/tads/fila.c include/tads/fila.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/tads/fila.c -o $(BUILD_DIR)/fila.o $(CFLAGS)

$(BUILD_DIR)/lista_seq.o: source/tads/lista_seq.c include/tads/lista_seq.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/tads/lista_seq.c -o $(BUILD_DIR)/lista_seq.o $(CFLAGS)

$(BUILD_DIR)/lista_enc.o: source/tads/lista_enc.c include/tads/lista_enc.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/tads/lista_enc.c -o $(BUILD_DIR)/lista_enc.o $(CFLAGS)

$(BUILD_DIR)/lista_cabeca.o: source/tads/lista_cabeca.c include/tads/lista_cabeca.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/tads/lista_cabeca.c -o $(BUILD_DIR)/lista_cabeca.o $(CFLAGS)

$(BUILD_DIR)/lista_ord.o: source/tads/lista_ord.c include/tads/lista_ord.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/tads/lista_ord.c -o $(BUILD_DIR)/lista_ord.o $(CFLAGS)

$(BUILD_DIR)/lista_gen.o: source/tads/lista_gen.c include/tads/lista_gen.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/tads/lista_gen.c -o $(BUILD_DIR)/lista_gen.o $(CFLAGS)

$(BUILD_DIR)/lista_cruz.o: source/tads/lista_cruz.c include/tads/lista_cruz.h include/item.h | $(BUILD_DIR)
	$(CC) -c source/tads/lista_cruz.c -o $(BUILD_DIR)/lista_cruz.o $(CFLAGS)

# executa um exemplo com a base inteira
run: $(TARGET)
	./$(TARGET) --reader antt --input dados/asteroids.csv --base lista_seq --result lista_ord --query consultas/q3.txt --output resultado.csv --top-k 20 --limit 10000
# executa os testes unitarios dos tads
teste:
	$(MAKE) -C tests run

# remove arquivos gerados
clean:
	rm -rf $(BUILD_DIR) resultado.csv
	$(MAKE) -C tests clean
