#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include "tads/pilha.h"

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
    if(pilha->item == NULL){
        free(pilha);
        return NULL;
    }

    return pilha;
}

// Libera a pilha. Se apagar_itens for true, os itens tambem sao apagados
static void pilha_liberar(PILHA **pilha, bool apagar_itens){
    if(pilha == NULL || *pilha == NULL){
        return;
    }
    if(apagar_itens){
        for(int i = 0; i < (*pilha)->topo; i++){
            item_apagar(&((*pilha)->item[i]));
        }
    }
    // primeiro libera o vetor, depois a struct que guarda o ponteiro dele
    free((*pilha)->item);
    free(*pilha);
    *pilha = NULL;
}

// Destroi a pilha e libera a memoria
void pilha_apagar(PILHA **pilha){
    pilha_liberar(pilha, true);
}

// Destroi so a pilha, os itens continuam existindo
void pilha_apagar_sem_itens(PILHA **pilha){
    pilha_liberar(pilha, false);
}

// Insere (empilha) um item no topo da pilha
bool pilha_empilhar(PILHA *pilha, ITEM *item){
    if(pilha == NULL || item == NULL){
        return false;
    }
    // vetor cheio: dobra a capacidade antes de inserir
    if(pilha->topo >= pilha->capacidade){
        int nova_capacidade = pilha->capacidade * 2;
        ITEM **aux = (ITEM **)realloc(pilha->item, nova_capacidade * sizeof(ITEM *));
        if(aux == NULL){
            return false; // o vetor antigo continua valido
        }
        pilha->item = aux;
        pilha->capacidade = nova_capacidade;
    }

    pilha->item[pilha->topo] = item;
    pilha->topo++;
    return true;
}

// Retira (desempilha) um item do topo da pilha
ITEM *pilha_desempilhar(PILHA *pilha){
    if(pilha == NULL || pilha->topo == 0){
        return NULL;
    }

    pilha->topo--;
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

// Permite consultar um item por indice (necessario para a busca)
// indice 0 = base da pilha, indice tamanho-1 = topo
ITEM *pilha_obter(const PILHA *pilha, int indice){
    if(pilha == NULL || indice < 0 || indice >= pilha->topo){
        return NULL;
    }
    return pilha->item[indice];
}
