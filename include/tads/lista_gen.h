#ifndef LISTA_GEN_H
#define LISTA_GEN_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_gen LISTA_GEN;

// cria a lista generalizada: itens organizados em grupos e, dentro de cada grupo, em subgrupos (ex: grupo = threat_category, subgrupo = year).
LISTA_GEN *lista_gen_criar(void);


void lista_gen_apagar(LISTA_GEN **lista); // destroi a lista e libera a memoria (incluindo os itens guardados nela)


void lista_gen_apagar_sem_itens(LISTA_GEN **lista); // destroi so a lista, os itens continuam existindo (usado na estrutura de resultado)




// insere o item no subgrupo do grupo indicado. se o grupo ou o subgrupo ainda nao existem, eles sao criados. os nomes sao textos, entao a lista funciona com qualquer campo de qualquer dataset (numeros devem ser convertidos antes).
bool lista_gen_inserir(LISTA_GEN *lista, ITEM *item, const char *nome_grupo, const char *nome_subgrupo);

// busca o primeiro item com o identificador informado (o item continua na lista)
ITEM *lista_gen_buscar(const LISTA_GEN *lista, const char *identificador);

// retorna a quantidade total de itens (somando todos os grupos)
int lista_gen_tamanho(const LISTA_GEN *lista);

// percurso: devolve os itens grupo por grupo e, dentro deles, subgrupo por subgrupo. grupos e subgrupos aparecem na ordem em que foram criados.
void lista_gen_reiniciar(LISTA_GEN *lista);
ITEM *lista_gen_proximo(LISTA_GEN *lista);

#endif
