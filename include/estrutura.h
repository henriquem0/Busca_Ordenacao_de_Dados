#ifndef ESTRUTURA_H
#define ESTRUTURA_H

#include <stdbool.h> 
#include "item.h"
#include "query.h"

/* ESTRUTURA e a "capa" que deixa o programa escolher, em tempo de execucao,
   qual TAD vai guardar os itens (--base e --result). O resto do programa
   so chama as funcoes estrutura_*, sem saber qual TAD esta por baixo. */
typedef struct estrutura_ ESTRUTURA; 

/* - Uso do "enum" a fim de facilitar e deixar mais visual 
   - a escolha de qual tipo de estrutura sera utilizado. */
typedef enum {
    ESTRUTURA_PILHA,
    ESTRUTURA_FILA,
    ESTRUTURA_LISTA_SEQ,
    ESTRUTURA_LISTA_ENC,
    ESTRUTURA_LISTA_CABECA,
    ESTRUTURA_LISTA_ORD,
    ESTRUTURA_LISTA_GEN,
    ESTRUTURA_LISTA_CRUZ
}TipoEstrutura;

// Converte o texto recebido na linha de comando ("pilha", "lista_enc"...) para o enum.
// Devolve false se o nome nao for de nenhuma estrutura.
bool estrutura_tipo_de_texto(const char *texto, TipoEstrutura *tipo);

/* Cria a estrutura do tipo pedido.
   - dono_dos_itens: true na base (apagar a estrutura apaga os itens), false no
     resultado (os itens sao os mesmos da base e nao podem ser apagados duas vezes)
   - query: consulta usada para ORDER (lista_ord), GROUP (lista_gen) e CROSS (lista_cruz) */
ESTRUTURA *estrutura_criar(TipoEstrutura tipo, bool dono_dos_itens, const QUERY *query);
void estrutura_apagar(ESTRUTURA **e);

/* Insere o item. A similaridade e a chave de ordem da lista_ord (nas outras
   estruturas ela e ignorada). Em lista_gen e lista_cruz os grupos saem dos
   campos GROUP e CROSS da consulta. */
bool estrutura_inserir(ESTRUTURA *e, ITEM *item, double similaridade);

// Remove o item da "frente" da estrutura (topo da pilha, inicio da fila ou primeiro da lista).
// Em lista_gen e lista_cruz a remocao nao foi implementada e devolve NULL.
ITEM *estrutura_remover(ESTRUTURA *e);

int estrutura_tamanho(const ESTRUTURA *e);

/* Percurso na ordem de acesso de cada estrutura (e a ordem em que o resultado
   e gravado no arquivo): pilha do topo para a base, fila e listas do inicio
   para o fim, lista_gen grupo por grupo e lista_cruz pelo primeiro campo do CROSS. */
void estrutura_reiniciar(ESTRUTURA *e);
ITEM *estrutura_proximo(ESTRUTURA *e);

#endif
