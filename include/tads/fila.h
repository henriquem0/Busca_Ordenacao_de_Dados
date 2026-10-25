#ifndef FILA_H
#define FILA_H

#include <stdbool.h>
#include "item.h"

typedef struct fila FILA;

// cria e inicializa a fila (vetor circular que cresce sozinho)
FILA *fila_criar(void);

// destroi a fila e libera a memoria (incluindo os itens guardados nela)
void fila_apagar(FILA **fila);

// destroi so a fila, os itens continuam existindo (usado na estrutura de resultado)
void fila_apagar_sem_itens(FILA **fila);

// insere um item no fim da fila
bool fila_inserir(FILA *fila, ITEM *item);

// retira e devolve o item do inicio da fila (quem chamou passa a ser dono do item)
ITEM *fila_remover(FILA *fila);

// retorna a quantidade atual de itens na fila
int fila_tamanho(const FILA *fila);

// consulta um item por posicao logica: 0 = inicio da fila, tamanho-1 = fim. o(1)
ITEM *fila_obter(const FILA *fila, int indice);

#endif
