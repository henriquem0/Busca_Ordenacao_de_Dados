#ifndef LISTA_ORD_H
#define LISTA_ORD_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_ord LISTA_ORD;

// Cria a lista ordenada. Se decrescente for true, o maior valor de chave fica
// no inicio (ORDER|similarity|DESC); senao, o menor fica no inicio.
LISTA_ORD *lista_ord_criar(bool decrescente);

// Destroi a lista e libera a memoria (incluindo os itens guardados nela)
void lista_ord_apagar(LISTA_ORD **lista);

// Destroi so a lista, os itens continuam existindo (usado na estrutura de resultado)
void lista_ord_apagar_sem_itens(LISTA_ORD **lista);

// Insere o item ja na posicao correta, mantendo a ordem pela chave (por exemplo,
// a similaridade). Chaves iguais ficam na ordem de chegada. Custo O(n).
bool lista_ord_inserir(LISTA_ORD *lista, ITEM *item, double chave);

// Busca o primeiro item com a designacao informada (o item continua na lista)
ITEM *lista_ord_buscar(const LISTA_ORD *lista, const char *designacao);

// Remove o primeiro item com a designacao informada e devolve ele
// (quem chamou passa a ser dono do item). Devolve NULL se nao achar.
// Depois de remover, o percurso volta para o inicio da lista.
ITEM *lista_ord_remover(LISTA_ORD *lista, const char *designacao);

// Retorna a quantidade atual de itens na lista
int lista_ord_tamanho(const LISTA_ORD *lista);

// Percurso: devolve os itens na ordem da lista. NULL quando acabam.
void lista_ord_reiniciar(LISTA_ORD *lista);
ITEM *lista_ord_proximo(LISTA_ORD *lista);

#endif
