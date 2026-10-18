#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tads/lista_ord.h"

typedef struct no NO;

struct no{
    ITEM *item;
    double chave; // valor usado para ordenar (ex: similaridade)
    NO *proximo;
};

struct lista_ord{
    NO *inicio;
    NO *fim;
    int tamanho;
    bool decrescente;
    NO *percurso; // cursor usado por reiniciar/proximo
};

// compara o identificador do item com o texto procurado
static bool mesmo_identificador(const ITEM *item, const char *identificador){
    const char *identificador_item = item_get_identificador(item);
    return identificador_item != NULL && strcmp(identificador_item, identificador) == 0;
}

// diz se um no que ja esta na lista deve ficar antes do item novo. chaves iguais ficam antes do novo, o que mantem a ordem de chegada.
static bool fica_antes(const LISTA_ORD *lista, double chave_existente, double chave_nova){
    if(lista->decrescente){
        return chave_existente >= chave_nova;
    }
    return chave_existente <= chave_nova;
}

// cria e inicializa a lista
LISTA_ORD *lista_ord_criar(bool decrescente){
    LISTA_ORD *lista = (LISTA_ORD *)malloc(sizeof(LISTA_ORD)); // reserva o controle da lista ordenada
    if(lista == NULL){
        return NULL;
    }
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    lista->decrescente = decrescente;
    lista->percurso = NULL;
    return lista;
}

// libera a lista. se apagar_itens for true, os itens tambem sao apagados
static void lista_ord_liberar(LISTA_ORD **lista, bool apagar_itens){
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
void lista_ord_apagar(LISTA_ORD **lista){
    lista_ord_liberar(lista, true);
}

// destroi so a lista, os itens continuam existindo
void lista_ord_apagar_sem_itens(LISTA_ORD **lista){
    lista_ord_liberar(lista, false);
}

// insercao ordenada: a lista ja esta ordenada, entao so precisamos achar o lugar do novo no. se ele vai depois do ultimo, entra direto pelo ponteiro fim (o(1)). senao, anda ate achar o primeiro no que deve ficar depois dele.
bool lista_ord_inserir(LISTA_ORD *lista, ITEM *item, double chave){
    if(lista == NULL || item == NULL){
        return false;
    }
    NO *novo = (NO *)malloc(sizeof(NO)); // cria o no que sera inserido
    if(novo == NULL){
        return false;
    }
    novo->item = item;
    novo->chave = chave; // guarda a chave usada na ordenacao
    novo->proximo = NULL;

    if(lista->fim == NULL || fica_antes(lista, lista->fim->chave, chave)){
        // vai para o fim da lista (inclui o caso da lista vazia)
        if(lista->inicio == NULL){
            lista->inicio = novo;
        }
        else{
            lista->fim->proximo = novo;
        }
        lista->fim = novo;
    }
    else{
        // vai antes do ultimo, entao o ponteiro fim nao muda
        NO *anterior = NULL;
        NO *atual = lista->inicio;
        while(atual != NULL && fica_antes(lista, atual->chave, chave)){
            anterior = atual;
            atual = atual->proximo;
        }
        novo->proximo = atual;
        if(anterior == NULL){
            lista->inicio = novo; // atualiza o primeiro no
        }
        else{
            anterior->proximo = novo; // encaixa o novo no entre os existentes
        }
    }
    lista->tamanho++;
    return true;
}

// busca sequencial pelo identificador (a chave de ordem nao e o identificador, entao nao da para parar antes do fim)
ITEM *lista_ord_buscar(const LISTA_ORD *lista, const char *identificador){
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

// remove pelo identificador, guardando o no anterior durante a busca
ITEM *lista_ord_remover(LISTA_ORD *lista, const char *identificador){
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
        lista->inicio = atual->proximo;
    }
    else{
        anterior->proximo = atual->proximo;
    }
    if(atual == lista->fim){
        lista->fim = anterior;
    }

    ITEM *removido = atual->item;
    free(atual);
    lista->tamanho--;
    lista->percurso = lista->inicio;
    return removido;
}

// retorna a quantidade atual de itens na lista
int lista_ord_tamanho(const LISTA_ORD *lista){
    if(lista == NULL){
        return 0;
    }
    return lista->tamanho;
}

// coloca o cursor no primeiro item
void lista_ord_reiniciar(LISTA_ORD *lista){
    if(lista != NULL){
        lista->percurso = lista->inicio;
    }
}

// devolve o item do cursor e avanca para o proximo no
ITEM *lista_ord_proximo(LISTA_ORD *lista){
    if(lista == NULL || lista->percurso == NULL){
        return NULL;
    }
    ITEM *item = lista->percurso->item;
    lista->percurso = lista->percurso->proximo; // avanca o cursor ordenado
    return item;
}
