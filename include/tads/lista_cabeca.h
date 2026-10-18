#ifndef LISTA_CABECA_H
#define LISTA_CABECA_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_cabeca LISTA_CABECA;

// cria a lista e o no cabeca (no estrutural que nao guarda nenhum item)
LISTA_CABECA *lista_cabeca_criar(void);

// destroi a lista e libera a memoria (incluindo os itens guardados nela)
void lista_cabeca_apagar(LISTA_CABECA **lista);

// destroi so a lista, os itens continuam existindo (usado na estrutura de resultado)
void lista_cabeca_apagar_sem_itens(LISTA_CABECA **lista);

// insere um item no fim da lista (custo o(1))
bool lista_cabeca_inserir(LISTA_CABECA *lista, ITEM *item);

// busca o primeiro item com o identificador informado (o item continua na lista)
ITEM *lista_cabeca_buscar(const LISTA_CABECA *lista, const char *identificador);

// remove o primeiro item com o identificador informado e devolve ele (quem chamou passa a ser dono do item). devolve null se nao achar. depois de remover, o percurso volta para o inicio da lista.
ITEM *lista_cabeca_remover(LISTA_CABECA *lista, const char *identificador);

// retorna a quantidade atual de itens na lista (o no cabeca nao conta)
int lista_cabeca_tamanho(const LISTA_CABECA *lista);

// percurso: reiniciar coloca o cursor no primeiro item (o proximo do cabeca) e cada chamada de proximo devolve um item e avanca. devolve null no fim.
void lista_cabeca_reiniciar(LISTA_CABECA *lista);
ITEM *lista_cabeca_proximo(LISTA_CABECA *lista);

#endif
