#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tads/fila.h"

#define FILA_CAPACIDADE_INICIAL 10

struct fila{
    ITEM **item;
    int inicio;     // posicao do proximo item a ser removido
    int fim;        // primeira posicao livre para inserir
    int tamanho;    // quantidade de itens guardados
    int capacidade; // tamanho alocado no vetor
};

// Cria e inicializa a fila
FILA *fila_criar(void){
    FILA *fila = (FILA *)malloc(sizeof(FILA));
    if(fila == NULL){
        return NULL;
    }
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;
    fila->capacidade = FILA_CAPACIDADE_INICIAL;

    fila->item = (ITEM **)malloc(fila->capacidade * sizeof(ITEM *));
    if(fila->item == NULL){
        free(fila);
        return NULL;
    }
    return fila;
}

// Dobra a capacidade da fila. Como o vetor e circular, os itens sao copiados
// na ordem logica (a partir de inicio) para o comeco do vetor novo.
static bool fila_aumentar(FILA *fila){
    int nova_capacidade = fila->capacidade * 2;
    ITEM **novo_vetor = (ITEM **)malloc(nova_capacidade * sizeof(ITEM *));
    if(novo_vetor == NULL){
        return false;
    }
    for(int i = 0; i < fila->tamanho; i++){
        novo_vetor[i] = fila->item[(fila->inicio + i) % fila->capacidade];
    }
    free(fila->item);
    fila->item = novo_vetor;
    fila->inicio = 0;
    fila->fim = fila->tamanho;
    fila->capacidade = nova_capacidade;
    return true;
}

// Libera a fila. Se apagar_itens for true, os itens tambem sao apagados
static void fila_liberar(FILA **fila, bool apagar_itens){
    if(fila == NULL || *fila == NULL){
        return;
    }
    if(apagar_itens){
        for(int i = 0; i < (*fila)->tamanho; i++){
            int posicao = ((*fila)->inicio + i) % (*fila)->capacidade;
            item_apagar(&((*fila)->item[posicao]));
        }
    }
    free((*fila)->item);
    free(*fila);
    *fila = NULL;
}

// Destroi a fila e libera a memoria
void fila_apagar(FILA **fila){
    fila_liberar(fila, true);
}

// Destroi so a fila, os itens continuam existindo
void fila_apagar_sem_itens(FILA **fila){
    fila_liberar(fila, false);
}

// Insere um item no fim da fila
bool fila_inserir(FILA *fila, ITEM *item){
    if(fila == NULL || item == NULL){
        return false;
    }
    if(fila->tamanho == fila->capacidade && !fila_aumentar(fila)){
        return false;
    }
    fila->item[fila->fim] = item;
    fila->fim = (fila->fim + 1) % fila->capacidade;
    fila->tamanho++;
    return true;
}

// Retira o item do inicio da fila
ITEM *fila_remover(FILA *fila){
    if(fila == NULL || fila->tamanho == 0){
        return NULL;
    }
    ITEM *item = fila->item[fila->inicio];
    fila->item[fila->inicio] = NULL;
    fila->inicio = (fila->inicio + 1) % fila->capacidade;
    fila->tamanho--;
    return item;
}

// Retorna a quantidade atual de itens na fila
int fila_tamanho(const FILA *fila){
    if(fila == NULL){
        return 0;
    }
    return fila->tamanho;
}

// Consulta um item por posicao logica (0 = inicio da fila)
ITEM *fila_obter(const FILA *fila, int indice){
    if(fila == NULL || indice < 0 || indice >= fila->tamanho){
        return NULL;
    }
    return fila->item[(fila->inicio + indice) % fila->capacidade];
}
