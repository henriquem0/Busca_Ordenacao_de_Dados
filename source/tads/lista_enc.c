#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tads/lista_enc.h"

typedef struct no NO;

struct no{
    ITEM *item;
    NO *proximo;
};

struct lista_enc{
    NO *inicio;
    NO *fim;
    int tamanho;
    NO *percurso; // cursor usado por reiniciar/proximo
};

// compara o identificador do item com o texto procurado
static bool mesmo_identificador(const ITEM *item, const char *identificador){
    const char *identificador_item = item_get_identificador(item);
    return identificador_item != NULL && strcmp(identificador_item, identificador) == 0;
}

// cria e inicializa a lista (vazia: inicio == fim == null)
LISTA_ENC *lista_enc_criar(void){
    LISTA_ENC *lista = (LISTA_ENC *)malloc(sizeof(LISTA_ENC)); // reserva o controle da lista
    if(lista == NULL){
        return NULL;
    }
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    lista->percurso = NULL;
    return lista;
}

// libera a lista. se apagar_itens for true, os itens tambem sao apagados
static void lista_enc_liberar(LISTA_ENC **lista, bool apagar_itens){
    if(lista == NULL || *lista == NULL){
        return;
    }
    NO *atual = (*lista)->inicio;
    while(atual != NULL){
        NO *proximo = atual->proximo; // guarda o proximo antes do free
        if(apagar_itens){
            item_apagar(&(atual->item));
        }
        free(atual);
        atual = proximo;
    }
    free(*lista);
    *lista = NULL;
}

// destroi a lista e libera a memoria
void lista_enc_apagar(LISTA_ENC **lista){
    lista_enc_liberar(lista, true);
}

// destroi so a lista, os itens continuam existindo
void lista_enc_apagar_sem_itens(LISTA_ENC **lista){
    lista_enc_liberar(lista, false);
}

// insere no fim da lista
bool lista_enc_inserir(LISTA_ENC *lista, ITEM *item){
    if(lista == NULL || item == NULL){
        return false;
    }
    NO *novo = (NO *)malloc(sizeof(NO)); // cria um no para o item
    if(novo == NULL){
        return false;
    }
    novo->item = item; // liga o item ao novo no
    novo->proximo = NULL;

    if(lista->inicio == NULL){
        lista->inicio = novo; // lista vazia: o novo no e o primeiro e o ultimo
    }
    else{
        lista->fim->proximo = novo; // liga o antigo ultimo ao novo no
    }
    lista->fim = novo;
    lista->tamanho++;
    return true;
}

// busca sequencial pelo identificador, custo o(n)
ITEM *lista_enc_buscar(const LISTA_ENC *lista, const char *identificador){
    if(lista == NULL || identificador == NULL){
        return NULL;
    }
    NO *atual = lista->inicio;
    while(atual != NULL){
        if(mesmo_identificador(atual->item, identificador)){
            return atual->item;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// remove pelo identificador. durante a busca guardamos o no anterior, porque em lista simples o no atual nao sabe quem vem antes dele.
ITEM *lista_enc_remover(LISTA_ENC *lista, const char *identificador){
    if(lista == NULL || identificador == NULL){
        return NULL;
    }
    NO *anterior = NULL;
    NO *atual = lista->inicio;
    while(atual != NULL && !mesmo_identificador(atual->item, identificador)){
        anterior = atual;
        atual = atual->proximo;
    }
    if(atual == NULL){
        return NULL; // nao encontrou
    }

    if(anterior == NULL){
        lista->inicio = atual->proximo; // removendo o primeiro
    }
    else{
        anterior->proximo = atual->proximo; // pula o no que sera removido
    }
    if(atual == lista->fim){
        lista->fim = anterior; // removendo o ultimo (fica null se era o unico)
    }

    ITEM *removido = atual->item;
    free(atual);
    lista->tamanho--;
    lista->percurso = lista->inicio;
    return removido;
}

// retorna a quantidade atual de itens na lista
int lista_enc_tamanho(const LISTA_ENC *lista){
    if(lista == NULL){
        return 0;
    }
    return lista->tamanho;
}

// coloca o cursor no primeiro item
void lista_enc_reiniciar(LISTA_ENC *lista){
    if(lista != NULL){
        lista->percurso = lista->inicio;
    }
}

// devolve o item do cursor e avanca para o proximo no
ITEM *lista_enc_proximo(LISTA_ENC *lista){
    if(lista == NULL || lista->percurso == NULL){
        return NULL;
    }
    ITEM *item = lista->percurso->item;
    lista->percurso = lista->percurso->proximo; // avanca o cursor para o proximo no
    return item;
}
