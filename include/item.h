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
// converte um texto qualquer para numero. aceita "true"/"false" (viram 1 e 0) e trata vírgula decimal. devolve false se o texto nao for numerico.
bool item_converter_numero(const char *texto, double *valor);
// converte o campo para numero (mesmas regras de item_converter_numero). devolve false se o campo esta ausente ou nao e numerico.
bool item_get_campo_numerico(const ITEM *item, const char *nome_campo, double *valor);

// 4) getters para as buscas e dados da ANTT
const char *item_get_consessionaria(const ITEM *item);
int item_get_id_sigicor(const ITEM *item); //devolve -1 em caso de nulo
const char *item_get_emissao_sei(const ITEM *item);
const char *item_get_autorizacao_sei(const ITEM *item);
int item_get_numero_sei(const ITEM *item); //devolve -1 em caso de nulo
const char *item_get_item_do_per(const ITEM *item);
const char *item_get_origem_per(const ITEM *item);
const char *item_get_tipo_obra(const ITEM *item);
const char *item_get_rodovia(const ITEM *item);

// Diferenciação de quilometragens (Contratual vs. Projeto Executivo)
double item_get_km_inicio(const ITEM *item); // km_Inicial (PER/Contrato) devolve -1.0 em caso de nulo
double item_get_km_final(const ITEM *item);  // km_Final (PER/Contrato) devolve -1.0 em caso de nulo
double item_get_KmInicio(const ITEM *item);  // km_inicial (Projeto Executivo) devolve -1.0 em caso de nulo
double item_get_KmFim(const ITEM *item);     // km_final (Projeto Executivo) devolve -1.0 em caso de nulo

const char *item_get_prazo(const ITEM *item);
const char *item_get_tipo_projeto(const ITEM *item);
const char *item_get_status_projeto(const ITEM *item);
const char *item_get_processo(const ITEM *item);

// - para identificacao: identificador do registro = id do item.
const char *item_get_identificador(const ITEM *item);

// 5) impressao: grava a linha original do registro (igual a do arquivo de entrada)
void item_imprimir_csv(const ITEM *item, FILE *fp);

#endif