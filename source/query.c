#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "query.h"

typedef struct{
    char campo_filter[64];
    char operador[3]; // "==", "!=", "<=", ">=", "<;", ">;" + '\0'
    char valor[64];
}FilterRule;

typedef struct{
    char campo_sim[64];
    double alvo;
    double tolerancia;
    double peso;
}SimRule;

struct query{
    FilterRule filtros[20];
    int qtda_filtros;

    SimRule sims[20];
    int qtda_sims;

    double limiar_accept;
    char campo_order[64]; 
    char sentido[8];

    char campo_cross[64];
    char ano[8];
    
};
// Cria e carrega as regras a partir do arquivo de consulta (.txt)
QUERY *query_carregar(const char *caminho_query);

// Apaga o TAD Query e libera a memoria
void query_apagar(QUERY **q); 

// Avalia um ITEM em relacao aos FILTES e SIMs da consulta
// Returna 'true' se o item for aceito e calcula a similaridade obtida
bool query_avaliar_item(QUERY *q, ITEM *item, double *similaridade_calc);