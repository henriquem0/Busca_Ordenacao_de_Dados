#ifndef LISTA_SEQ_H
#define LISTA_SEQ_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_seq LISTA_SEQ;


LISTA_SEQ *lista_seq_criar(void); // cria e inicializa a lista sequencial


void lista_seq_apagar(LISTA_SEQ **lista); // destroi a lista e libera a memoria (incluindo os itens guardados nela)


void lista_seq_apagar_sem_itens(LISTA_SEQ **lista);// destroi so a lista; os itens continuam existindo (usado na estrutura de resultado)

// insere um item no fim da lista
bool lista_seq_inserir(LISTA_SEQ *lista, ITEM *item);

// busca o primeiro item com o identificador informado (o item continua na lista)
ITEM *lista_seq_buscar(const LISTA_SEQ *lista, const char *identificador);

// remove o primeiro item com o identificador informado e devolve ele (quem chamou passa a ser dono do item). devolve null se nao achar
ITEM *lista_seq_remover(LISTA_SEQ *lista, const char *identificador);


int lista_seq_tamanho(const LISTA_SEQ *lista);// retorna a quantidade atual de itens na lista

// consulta um item por indice; acesso direto em o(1)
ITEM *lista_seq_obter(const LISTA_SEQ *lista, int indice);

#endif
