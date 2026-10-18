#ifndef ITEM_H
#define ITEM_H

#include <stdio.h>
#include <stdbool.h>

typedef struct item_ ITEM;

// o item guarda as colunas de uma linha do csv como texto e a linha original. os nomes das colunas vem do cabecalho do arquivo, que o leitor informa uma vez, antes de criar os itens. assim o mesmo item serve para qualquer dataset.

// 1) cabecalho do dataset (nomes das colunas)
void item_definir_cabecalho(const char *linha_cabecalho);
const char *item_get_cabecalho(void);   // cabecalho sem o fim de linha
const char *item_get_terminador(void);  // fim de linha do arquivo ("\n" ou "\r\n")
void item_liberar_cabecalho(void);

// 2) construtor e destruidor
ITEM *item_criar(const char *linha_csv);
void item_apagar(ITEM **item);

// 3) acesso generico pelo nome da coluna (usado por filter, sim, order, group e cross) devolve null se a coluna nao existe ou se o valor esta vazio (valor ausente)
const char *item_get_campo(const ITEM *item, const char *nome_campo);
// converte um texto qualquer para numero. aceita "true"/"false" (viram 1 e 0). devolve false se o texto nao for numerico.
bool item_converter_numero(const char *texto, double *valor);
// converte o campo para numero (mesmas regras de item_converter_numero). devolve false se o campo esta ausente ou nao e numerico.
bool item_get_campo_numerico(const ITEM *item, const char *nome_campo, double *valor);

// 4) getters para as buscas

// - filtros (filter)
double item_get_distancia(const ITEM *item);
bool item_get_is_future_event(const ITEM *item);

// - similaridades (sim)
double item_get_velocidade(const ITEM *item);
double item_get_risk_score(const ITEM *item);

// - por agrupamento e listas avancadas (group e cross)
const char *item_get_threat_category(const ITEM *item);
int item_get_year(const ITEM *item);

// - para identificacao identificador do registro = valor da primeira coluna, seja qual for o dataset. e o que os tads usam em buscar/remover, assim eles nao dependem de nenhum dataset.
const char *item_get_identificador(const ITEM *item);
// na nasa, o identificador e a coluna asteroid_designation
const char *item_get_designation(const ITEM *item);

// 5) impressao: grava a linha original do registro (igual a do arquivo de entrada)
void item_imprimir_csv(const ITEM *item, FILE *fp);

#endif
