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

// Compara a designacao do item com o texto procurado
static bool mesma_designacao(const ITEM *item, const char *designacao){
    const char *designacao_item = item_get_designation(item);
    return designacao_item != NULL && strcmp(designacao_item, designacao) == 0;
}

// Procura a posicao do primeiro item com a designacao. Devolve -1 se nao achar.
static int lista_seq_posicao(const LISTA_SEQ *lista, const char *designacao){
    for(int i = 0; i < lista->tamanho; i++){
        if(mesma_designacao(lista->item[i], designacao)){
            return i;
        }
    }
    return -1;
}

// Cria e inicializa a lista
LISTA_SEQ *lista_seq_criar(void){
    LISTA_SEQ *lista = (LISTA_SEQ *)malloc(sizeof(LISTA_SEQ));
    if(lista == NULL){
        return NULL;
    }
    lista->tamanho = 0;
    lista->capacidade = LISTA_SEQ_CAPACIDADE_INICIAL;

    lista->item = (ITEM **)malloc(lista->capacidade * sizeof(ITEM *));
    if(lista->item == NULL){
        free(lista);
        return NULL;
    }
    return lista;
}

// Libera a lista. Se apagar_itens for true, os itens tambem sao apagados
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

// Destroi a lista e libera a memoria
void lista_seq_apagar(LISTA_SEQ **lista){
    lista_seq_liberar(lista, true);
}

// Destroi so a lista, os itens continuam existindo
void lista_seq_apagar_sem_itens(LISTA_SEQ **lista){
    lista_seq_liberar(lista, false);
}

// Insere um item no fim da lista (custo O(1), O(n) so quando precisa crescer)
bool lista_seq_inserir(LISTA_SEQ *lista, ITEM *item){
    if(lista == NULL || item == NULL){
        return false;
    }
    if(lista->tamanho >= lista->capacidade){
        int nova_capacidade = lista->capacidade * 2;
        ITEM **aux = (ITEM **)realloc(lista->item, nova_capacidade * sizeof(ITEM *));
        if(aux == NULL){
            return false; // o vetor antigo continua valido
        }
        lista->item = aux;
        lista->capacidade = nova_capacidade;
    }
    lista->item[lista->tamanho] = item;
    lista->tamanho++;
    return true;
}

// Busca sequencial pela designacao, custo O(n)
ITEM *lista_seq_buscar(const LISTA_SEQ *lista, const char *designacao){
    if(lista == NULL || designacao == NULL){
        return NULL;
    }
    int posicao = lista_seq_posicao(lista, designacao);
    if(posicao < 0){
        return NULL;
    }
    return lista->item[posicao];
}

// Remove pela designacao. Os itens da direita deslocam uma posicao para a esquerda,
// por isso o custo e O(n).
ITEM *lista_seq_remover(LISTA_SEQ *lista, const char *designacao){
    if(lista == NULL || designacao == NULL){
        return NULL;
    }
    int posicao = lista_seq_posicao(lista, designacao);
    if(posicao < 0){
        return NULL;
    }
    ITEM *removido = lista->item[posicao];
    for(int i = posicao; i < lista->tamanho - 1; i++){
        lista->item[i] = lista->item[i + 1];
    }
    lista->tamanho--;
    lista->item[lista->tamanho] = NULL;
    return removido;
}

// Retorna a quantidade atual de itens na lista
int lista_seq_tamanho(const LISTA_SEQ *lista){
    if(lista == NULL){
        return 0;
    }
    return lista->tamanho;
}

// Consulta um item por indice
ITEM *lista_seq_obter(const LISTA_SEQ *lista, int indice){
    if(lista == NULL || indice < 0 || indice >= lista->tamanho){
        return NULL;
    }
    return lista->item[indice];
}
