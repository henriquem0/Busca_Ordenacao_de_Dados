#ifndef LISTA_ENC_H
#define LISTA_ENC_H

#include <stdbool.h>
#include "item.h"

/*
 * LISTA ENCADEADA
 *
 * A lista encadeada organiza os elementos por meio de nós
 * conectados por ponteiros. Cada nó armazena um registro
 * ou uma referência ao registro e um ponteiro para o próximo nó.
 *
 * Os elementos não precisam ocupar posições consecutivas
 * na memória. A inserção e a remoção devem atualizar os
 * ponteiros envolvidos, preservando a ligação entre os nós.
 *
 * A busca e o percurso devem seguir os encadeamentos da lista,
 * partindo do primeiro nó e avançando até encontrar o elemento
 * procurado ou alcançar o final da estrutura.
 */

typedef struct lista_enc LISTA_ENC;

// cria e inicializa a lista simplesmente encadeada
LISTA_ENC *lista_enc_criar(void);

// destroi a lista e libera a memoria (incluindo os itens guardados nela)
void lista_enc_apagar(LISTA_ENC **lista);

// destroi so a lista, os itens continuam existindo (usado na estrutura de resultado)
void lista_enc_apagar_sem_itens(LISTA_ENC **lista);

// insere um item no fim da lista (custo o(1) porque a lista guarda o ponteiro fim)
bool lista_enc_inserir(LISTA_ENC *lista, ITEM *item);

// busca o primeiro item com o identificador informado (o item continua na lista)
ITEM *lista_enc_buscar(const LISTA_ENC *lista, const char *identificador);

// remove o primeiro item com o identificador informado e devolve ele (quem chamou passa a ser dono do item). devolve null se nao achar. depois de remover, o percurso volta para o inicio da lista.
ITEM *lista_enc_remover(LISTA_ENC *lista, const char *identificador);

// retorna a quantidade atual de itens na lista
int lista_enc_tamanho(const LISTA_ENC *lista);

// percurso: reiniciar coloca o cursor no primeiro item e cada chamada de proximo devolve um item e avanca. devolve null quando acabam os itens.
void lista_enc_reiniciar(LISTA_ENC *lista);
ITEM *lista_enc_proximo(LISTA_ENC *lista);

#endif
