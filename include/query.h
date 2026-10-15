#ifndef QUERY_H
#define QUERY_H

#include <stdbool.h>
#include "item.h"

typedef struct query QUERY;

// Cria e carrega as regras a partir do arquivo de consulta (.txt)
QUERY *query_carregar(const char *caminho_query);

// Apaga o TAD Query e libera a memoria
void query_apagar(QUERY **q); 

// Avalia um ITEM em relacao aos FILTES e SIMs da consulta
// Returna 'true' se o item for aceito e calcula a similaridade obtida
bool query_avaliar_item(QUERY *q, ITEM *item, double *similaridade_calc);

#endif