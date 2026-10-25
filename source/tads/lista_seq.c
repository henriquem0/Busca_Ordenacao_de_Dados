#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tads/lista_seq.h"

#define LISTA_SEQ_CAPACIDADE_INICIAL 10

struct lista_seq{
    ITEM **item;
    int tamanho;    // quantidade de itens guardados
    int capacidade; // tamanho alocado no vetor
};

// compara o identificador do item com o texto procurado
static bool mesmo_identificador(const ITEM *item, const char *identificador){
    const char *identificador_item = item_get_identificador(item);
    return identificador_item != NULL && strcmp(identificador_item, identificador) == 0;
}

// procura a posicao do primeiro item com o identificador. devolve -1 se nao achar.
static int lista_seq_posicao(const LISTA_SEQ *lista, const char *identificador){
    for(int i = 0; i < lista->tamanho; i++){
        if(mesmo_identificador(lista->item[i], identificador)){
            return i;
        }
    }
    return -1;
}

// cria e inicializa a lista
LISTA_SEQ *lista_seq_criar(void){
    LISTA_SEQ *lista = (LISTA_SEQ *)malloc(sizeof(LISTA_SEQ)); // reserva o controle da lista
    if(lista == NULL){
        return NULL;
    }
    lista->tamanho = 0;
    lista->capacidade = LISTA_SEQ_CAPACIDADE_INICIAL;

    lista->item = (ITEM **)malloc(lista->capacidade * sizeof(ITEM *)); // reserva o vetor de itens
    if(lista->item == NULL){
        free(lista);
        return NULL;
    }
    return lista;
}

// libera a lista. se apagar_itens for true, os itens tambem sao apagados
static void lista_seq_liberar(LISTA_SEQ **lista, bool apagar_itens){
    if(lista == NULL || *lista == NULL){
        return;
    }
    if(apagar_itens){
        for(int i = 0; i < (*lista)->tamanho; i++){
            item_apagar(&((*lista)->item[i]));
        }
    }
    free((*lista)->item);
    free(*lista);
    *lista = NULL;
}

// destroi a lista e libera a memoria
void lista_seq_apagar(LISTA_SEQ **lista){
    lista_seq_liberar(lista, true);
}

// destroi so a lista, os itens continuam existindo
void lista_seq_apagar_sem_itens(LISTA_SEQ **lista){
    lista_seq_liberar(lista, false);
}

// insere um item no fim da lista (custo o(1), o(n) so quando precisa crescer)
bool lista_seq_inserir(LISTA_SEQ *lista, ITEM *item){
    if(lista == NULL || item == NULL){
        return false;
    }
    if(lista->tamanho >= lista->capacidade){
        int nova_capacidade = lista->capacidade * 2;
        ITEM **aux = (ITEM **)realloc(lista->item, nova_capacidade * sizeof(ITEM *)); // aumenta o vetor sem perder os itens
        if(aux == NULL){
            return false; // o vetor antigo continua valido
        }
        lista->item = aux;
        lista->capacidade = nova_capacidade;
    }
    lista->item[lista->tamanho] = item; // coloca o novo item no fim
    lista->tamanho++;
    return true;
}

// busca sequencial pelo identificador, custo o(n)
ITEM *lista_seq_buscar(const LISTA_SEQ *lista, const char *identificador){
    if(lista == NULL || identificador == NULL){
        return NULL;
    }
    int posicao = lista_seq_posicao(lista, identificador);
    if(posicao < 0){
        return NULL;
    }
    return lista->item[posicao];
}

// remove pelo identificador. os itens da direita deslocam uma posicao para a esquerda, por isso o custo e o(n).
ITEM *lista_seq_remover(LISTA_SEQ *lista, const char *identificador){
    if(lista == NULL || identificador == NULL){
        return NULL;
    }
    int posicao = lista_seq_posicao(lista, identificador);
    if(posicao < 0){
        return NULL;
    }
    ITEM *removido = lista->item[posicao];
    for(int i = posicao; i < lista->tamanho - 1; i++){
        lista->item[i] = lista->item[i + 1]; // desloca os itens para fechar o espaco
    }
    lista->tamanho--;
    lista->item[lista->tamanho] = NULL;
    return removido;
}

// retorna a quantidade atual de itens na lista
int lista_seq_tamanho(const LISTA_SEQ *lista){
    if(lista == NULL){
        return 0;
    }
    return lista->tamanho;
}

// consulta um item por indice
ITEM *lista_seq_obter(const LISTA_SEQ *lista, int indice){
    if(lista == NULL || indice < 0 || indice >= lista->tamanho){
        return NULL;
    }
    return lista->item[indice]; // acessa diretamente a posicao solicitada
}
