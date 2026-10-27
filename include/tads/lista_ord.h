#ifndef LISTA_ORD_H
#define LISTA_ORD_H

#include <stdbool.h>
#include "item.h"

/*
 * LISTA ORDENADA
 *
 * Os elementos devem permanecer ordenados de acordo com o critério
 * definido pela implementação. A posição de inserção deve ser
 * determinada no momento em que cada elemento é inserido.
 *
 * Não é permitido inserir todos os elementos e ordenar a lista
 * somente ao final do processamento.
 */

typedef struct lista_ord LISTA_ORD;

// cria a lista ordenada. se decrescente for true, o maior valor de chave fica no inicio (order|similarity|desc); senao, o menor fica no inicio.
LISTA_ORD *lista_ord_criar(bool decrescente);

// destroi a lista e libera a memoria (incluindo os itens guardados nela)
void lista_ord_apagar(LISTA_ORD **lista);

// destroi so a lista, os itens continuam existindo (usado na estrutura de resultado)
void lista_ord_apagar_sem_itens(LISTA_ORD **lista);

// insere o item ja na posicao correta, mantendo a ordem pela chave (por exemplo, a similaridade). chaves iguais ficam na ordem de chegada. custo o(n).
bool lista_ord_inserir(LISTA_ORD *lista, ITEM *item, double chave);

// busca o primeiro item com o identificador informado (o item continua na lista)
ITEM *lista_ord_buscar(const LISTA_ORD *lista, const char *identificador);

// remove o primeiro item com o identificador informado e devolve ele (quem chamou passa a ser dono do item). devolve null se nao achar. depois de remover, o percurso volta para o inicio da lista.
ITEM *lista_ord_remover(LISTA_ORD *lista, const char *identificador);

// retorna a quantidade atual de itens na lista
int lista_ord_tamanho(const LISTA_ORD *lista);

// percurso: devolve os itens na ordem da lista. null quando acabam.
void lista_ord_reiniciar(LISTA_ORD *lista);
ITEM *lista_ord_proximo(LISTA_ORD *lista);

#endif
