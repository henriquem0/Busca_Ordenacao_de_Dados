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

// Campo usado pela lista ordenada (ORDER). Vazio ou "similarity" = ordenar pela similaridade.
const char *query_get_campo_order(const QUERY *q);
// true se a ordem pedida e DESC (padrao quando nao ha ORDER)
bool query_order_decrescente(const QUERY *q);

// Campos de agrupamento (GROUP) e de cruzamento (CROSS). nivel = 1 ou 2.
// Devolvem texto vazio se o campo nao foi informado na consulta.
const char *query_get_campo_group(const QUERY *q, int nivel);
const char *query_get_campo_cross(const QUERY *q, int nivel);

#endif
