#ifndef ITEM_H
#define ITEM_H

#include <stdio.h>
#include <stdbool.h>

typedef struct item_ ITEM;

/* O ITEM guarda as colunas de uma linha do CSV como texto e a linha original.
   Os nomes das colunas vem do cabecalho do arquivo, que o leitor informa uma
   vez, antes de criar os itens. Assim o mesmo ITEM serve para qualquer dataset. */

// 1) Cabecalho do dataset (nomes das colunas)
void item_definir_cabecalho(const char *linha_cabecalho);
const char *item_get_cabecalho(void);   // cabecalho sem o fim de linha
const char *item_get_terminador(void);  // fim de linha do arquivo ("\n" ou "\r\n")
void item_liberar_cabecalho(void);

// 2) Construtor e Destruidor
ITEM *item_criar(const char *linha_csv);
void item_apagar(ITEM **item);

// 3) Acesso generico pelo nome da coluna (usado por FILTER, SIM, ORDER, GROUP e CROSS)
// Devolve NULL se a coluna nao existe ou se o valor esta vazio (valor ausente)
const char *item_get_campo(const ITEM *item, const char *nome_campo);
// Converte um texto qualquer para numero. Aceita "true"/"false" (viram 1 e 0).
// Devolve false se o texto nao for numerico.
bool item_converter_numero(const char *texto, double *valor);
// Converte o campo para numero (mesmas regras de item_converter_numero).
// Devolve false se o campo esta ausente ou nao e numerico.
bool item_get_campo_numerico(const ITEM *item, const char *nome_campo, double *valor);

// 4) GETTERS para as buscas

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
// Identificador do registro = valor da PRIMEIRA coluna, seja qual for o dataset.
// E o que os TADs usam em buscar/remover, assim eles nao dependem de nenhum dataset.
const char *item_get_identificador(const ITEM *item);
// Na NASA, o identificador e a coluna asteroid_designation
const char *item_get_designation(const ITEM *item);

// 5) Impressao: grava a linha original do registro (igual a do arquivo de entrada)
void item_imprimir_csv(const ITEM *item, FILE *fp);

#endif
