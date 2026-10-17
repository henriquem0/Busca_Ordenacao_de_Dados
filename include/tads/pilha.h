#ifndef PILHA_H
#define PILHA_H

#include <stdbool.h> 
#include "item.h"

typedef struct pilha PILHA;

// Cria e inicializa a pilha
PILHA *pilha_criar(void);

// Destroi a pilha e libera a memoria (incluindo os itens guardados nela)
void pilha_apagar(PILHA **pilha);

// Destroi so a pilha, os itens continuam existindo (usado na estrutura de resultado)
void pilha_apagar_sem_itens(PILHA **pilha);

// Insere (empilha) um item no topo da pilha
bool pilha_empilhar(PILHA *pilha, ITEM *item);

// Retira (desempilha) um item do topo da pilha
ITEM *pilha_desempilhar(PILHA *pilha);

// Retorna a quantidade atual de itens na pilha
int pilha_tamanho(const PILHA *pilha);

// Permite consultar um item por indicie(necessario para a busca)
ITEM *pilha_obter(const PILHA *pilha, int indice);

#endif