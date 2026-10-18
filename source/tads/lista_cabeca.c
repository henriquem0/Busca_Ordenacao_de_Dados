#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tads/lista_cabeca.h"

typedef struct no NO;

struct no{
    ITEM *item; // no cabeca guarda null aqui: ele nao representa um registro
    NO *proximo;
};

// invariantes da lista: - cabeca existe enquanto a lista existir (nunca e null) - lista vazia: cabeca->proximo == null, fim == cabeca e tamanho == 0 - lista nao vazia: cabeca->proximo e o primeiro item e fim e o ultimo no com fim == cabeca na lista vazia, inserir sempre faz fim->proximo = novo, sem precisar de um caso especial para o primeiro elemento.
struct lista_cabeca{
    NO *cabeca;
    NO *fim;
    int tamanho;
    NO *percurso; // cursor usado por reiniciar/proximo
};

// compara o identificador do item com o texto procurado
static bool mesmo_identificador(const ITEM *item, const char *identificador){
    const char *identificador_item = item_get_identificador(item);
    return identificador_item != NULL && strcmp(identificador_item, identificador) == 0;
}

// cria a lista com o no cabeca
LISTA_CABECA *lista_cabeca_criar(void){
    LISTA_CABECA *lista = (LISTA_CABECA *)malloc(sizeof(LISTA_CABECA)); // reserva o controle da lista
    if(lista == NULL){
        return NULL;
    }
    lista->cabeca = (NO *)malloc(sizeof(NO)); // cria o no sentinela
    if(lista->cabeca == NULL){
        free(lista); // desfaz a primeira alocacao
        return NULL;
    }
    lista->cabeca->item = NULL;
    lista->cabeca->proximo = NULL;
    lista->fim = lista->cabeca;
    lista->tamanho = 0;
    lista->percurso = NULL;
    return lista;
}

// libera a lista. se apagar_itens for true, os itens tambem sao apagados
static void lista_cabeca_liberar(LISTA_CABECA **lista, bool apagar_itens){
    if(lista == NULL || *lista == NULL){
        return;
    }
    NO *atual = (*lista)->cabeca->proximo;
    while(atual != NULL){
        NO *proximo = atual->proximo; // guarda o proximo antes do free
        if(apagar_itens){
            item_apagar(&(atual->item));
        }
        free(atual);
        atual = proximo;
    }
    free((*lista)->cabeca);
    free(*lista);
    *lista = NULL;
}

// destroi a lista e libera a memoria
void lista_cabeca_apagar(LISTA_CABECA **lista){
    lista_cabeca_liberar(lista, true);
}

// destroi so a lista, os itens continuam existindo
void lista_cabeca_apagar_sem_itens(LISTA_CABECA **lista){
    lista_cabeca_liberar(lista, false);
}

// insere no fim da lista
bool lista_cabeca_inserir(LISTA_CABECA *lista, ITEM *item){
    if(lista == NULL || item == NULL){
        return false;
    }
    NO *novo = (NO *)malloc(sizeof(NO));
    if(novo == NULL){
        return false;
    }
    novo->item = item;
    novo->proximo = NULL;

    lista->fim->proximo = novo; // liga o ultimo no ao novo item
    lista->fim = novo;
    lista->tamanho++;
    return true;
}

// busca sequencial pelo identificador, comecando depois do no cabeca
ITEM *lista_cabeca_buscar(const LISTA_CABECA *lista, const char *identificador){
    if(lista == NULL || identificador == NULL){
        return NULL;
    }
    NO *atual = lista->cabeca->proximo;
    while(atual != NULL){
        if(mesmo_identificador(atual->item, identificador)){
            return atual->item;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// remove pelo identificador. a busca procura o anterior do no que sera removido, comecando na cabeca. assim remover o primeiro item e igual a remover qualquer outro: nao existe caso especial.
ITEM *lista_cabeca_remover(LISTA_CABECA *lista, const char *identificador){
    if(lista == NULL || identificador == NULL){
        return NULL;
    }
    NO *anterior = lista->cabeca;
    while(anterior->proximo != NULL &&
          !mesmo_identificador(anterior->proximo->item, identificador)){
        anterior = anterior->proximo;
    }
    NO *removido = anterior->proximo;
    if(removido == NULL){
        return NULL; // nao encontrou
    }

    anterior->proximo = removido->proximo; // retira o no da cadeia
    if(removido == lista->fim){
        lista->fim = anterior; // vira a cabeca se a lista ficou vazia
    }

    ITEM *item = removido->item;
    free(removido);
    lista->tamanho--;
    lista->percurso = lista->cabeca->proximo;
    return item;
}

// retorna a quantidade atual de itens na lista
int lista_cabeca_tamanho(const LISTA_CABECA *lista){
    if(lista == NULL){
        return 0;
    }
    return lista->tamanho;
}

// coloca o cursor no primeiro item (o no depois da cabeca)
void lista_cabeca_reiniciar(LISTA_CABECA *lista){
    if(lista != NULL){
        lista->percurso = lista->cabeca->proximo;
    }
}

// devolve o item do cursor e avanca para o proximo no
ITEM *lista_cabeca_proximo(LISTA_CABECA *lista){
    if(lista == NULL || lista->percurso == NULL){
        return NULL;
    }
    ITEM *item = lista->percurso->item;
    lista->percurso = lista->percurso->proximo; // avanca o cursor da lista
    return item;
}
