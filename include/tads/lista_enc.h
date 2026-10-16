#ifndef LISTA_ENC_H
#define LISTA_ENC_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_enc LISTA_ENC;

// Cria e inicializa a lista simplesmente encadeada
LISTA_ENC *lista_enc_criar(void);

// Destroi a lista e libera a memoria (incluindo os itens guardados nela)
void lista_enc_apagar(LISTA_ENC **lista);

// Destroi so a lista, os itens continuam existindo (usado na estrutura de resultado)
void lista_enc_apagar_sem_itens(LISTA_ENC **lista);

// Insere um item no fim da lista (custo O(1) porque a lista guarda o ponteiro fim)
bool lista_enc_inserir(LISTA_ENC *lista, ITEM *item);

// Busca o primeiro item com a designacao informada (o item continua na lista)
ITEM *lista_enc_buscar(const LISTA_ENC *lista, const char *designacao);

// Remove o primeiro item com a designacao informada e devolve ele
// (quem chamou passa a ser dono do item). Devolve NULL se nao achar.
// Depois de remover, o percurso volta para o inicio da lista.
ITEM *lista_enc_remover(LISTA_ENC *lista, const char *designacao);

// Retorna a quantidade atual de itens na lista
int lista_enc_tamanho(const LISTA_ENC *lista);

// Percurso: reiniciar coloca o cursor no primeiro item e cada chamada de
// proximo devolve um item e avanca. Devolve NULL quando acabam os itens.
void lista_enc_reiniciar(LISTA_ENC *lista);
ITEM *lista_enc_proximo(LISTA_ENC *lista);

#endif
