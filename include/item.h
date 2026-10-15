#ifndef ITEM_H
#define ITEM_H

#include <stdio.h>
#include <stdbool.h>

typedef struct item_ ITEM;

// 1) Construtor e Destruidor
ITEM *item_criar(const char *linha_csv);
void item_apagar(ITEM **item);

// 2) GETTERS para as buscas

// - Filtros (FILTER)
double item_get_distancia(const ITEM *item);
bool item_get_is_future_event(const ITEM *item);

// - Similaridades (SIM)
double item_get_velocidade(const ITEM *item);
double item_get_risk_score(const ITEM *item);

// - Por Agrupamento e Listas Avancadas (GROUP e CROSS)
const char *item_get_threat_category(const ITEM *item);
int item_get_year(const ITEM *item);

// - Para identificacao
const char *item_get_designation(const ITEM *item);

// 3) Impressao
void item_imprimir_csv(const ITEM *item, FILE *fp);

#endif