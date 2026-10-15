#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include "pilha.h"

struct pilha{
    ITEM **item;     
    int topo;       //topo atual
    int capacidade; //tamanho alocado no vetor
};

// Cria e inicializa a pilha
PILHA *pilha_criar(void){
    PILHA *pilha = (PILHA *)malloc(sizeof(PILHA));
    if(pilha == NULL){
        return NULL;
    }
    pilha->topo = 0; 
    pilha->capacidade = 10;

    pilha->item = (ITEM **)malloc(pilha->capacidade * sizeof(ITEM*));
    if(item == NULL){
        free(pilha)
        return NULL;
    }

    return pilha;
}

// Destroi a pilha e libera a memoria
void pilha_apagar(PILHA **pilha){
    if(pilha == NULL || *pilha == NULL){
        return;
    }
    for(int i = 0; i < (*pilha)->topo; i++){
        item_apagar(&((*pilha)->item[i]));
    }
    free(*pilha);
    free((*pilha)->item);
    *pilha = NULL;
}

// Insere (empilha) um item no topo da pilha
bool pilha_empilhar(PILHA *pilha, ITEM *item)|{
    if(pilha == NULL || item == NULL){
        return false;
    }
    if(pilha->capacidade <= pilha->topo){
        pilha->capacidade * 2;
        ITEM **aux = (ITEM **)realloc(pilha->capacidade * sizeof(ITEM *));
        if(aux == NULL){
            return false;
        }

        pilha->item = aux;
    }

    pilha->item[pilha->topo] = item;
    pilha->topo++;
    return true;
}

// Retira (desempilha) um item do topo da pilha
ITEM *pilha_desempilhar(PILHA *pilha){
    if(pilha == NULL || pilha->topo = 0){
        return NULL;
    }

    item->topo--;
    ITEM *item = pilha->item[pilha->topo];
    pilha->item[pilha->topo] = NULL;
    return item; 
}

// Retorna a quantidade atual de itens na pilha
int pilha_tamanho(const PILHA *pilha){
    if(pilha == NULL){
        return 0;
    }
    return pilha->topo;
}

// Permite consultar um item por indicie(necessario para a busca)
ITEM *pilha_obter(const PILHA *pilha, ind indice){
    if(pilha == NULL || indice < 0 || indice >= pilha->topo){
        return NULL
    }
    return pilha->item[indice];
}