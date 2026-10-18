#ifndef QUERY_H
#define QUERY_H

#include <stdbool.h>
#include "item.h"

typedef struct query QUERY;

// cria e carrega as regras a partir do arquivo de consulta (.txt)
QUERY *query_carregar(const char *caminho_query);

// apaga o tad query e libera a memoria
void query_apagar(QUERY **q); 

// avalia um item em relacao aos filtes e sims da consulta returna 'true' se o item for aceito e calcula a similaridade obtida
bool query_avaliar_item(QUERY *q, ITEM *item, double *similaridade_calc);

// campo usado pela lista ordenada (order). vazio ou "similarity" = ordenar pela similaridade.
const char *query_get_campo_order(const QUERY *q);
// true se a ordem pedida e desc (padrao quando nao ha order)
bool query_order_decrescente(const QUERY *q);

// campos de agrupamento (group) e de cruzamento (cross). nivel = 1 ou 2. devolvem texto vazio se o campo nao foi informado na consulta.
const char *query_get_campo_group(const QUERY *q, int nivel);
const char *query_get_campo_cross(const QUERY *q, int nivel);

#endif
