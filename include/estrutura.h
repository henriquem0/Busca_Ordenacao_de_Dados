#ifndef ESTRUTURA_H
#define ESTRUTURA_H

#include <stdbool.h> 
#include "item.h"
#include "query.h"

// strutura e a "capa" que deixa o programa escolher, em tempo de execucao, qual tad vai guardar os itens (--base e --result). o resto do programa so chama as funcoes estrutura_*, sem saber qual tad esta por baixo.
typedef struct estrutura_ ESTRUTURA; 

//uso do enum pra facilitar e deixar mais visual a escolha de qual tipo de estrutura sera utilizado.
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

// converte o texto recebido na linha de comando ("pilha", "lista_enc"...) para o enum. devolve false se o nome nao for de nenhuma estrutura.
bool estrutura_tipo_de_texto(const char *texto, TipoEstrutura *tipo);

// cria a estrutura do tipo pedido. - dono_dos_itens: true na base (apagar a estrutura apaga os itens), false no resultado (os itens sao os mesmos da base e nao podem ser apagados duas vezes) - query: consulta usada para order (lista_ord), group (lista_gen) e cross (lista_cruz)
ESTRUTURA *estrutura_criar(TipoEstrutura tipo, bool dono_dos_itens, const QUERY *query);
void estrutura_apagar(ESTRUTURA **e);

// insere o item. a similaridade e a chave de ordem da lista_ord (nas outras estruturas ela e ignorada). em lista_gen e lista_cruz os grupos saem dos campos group e cross da consulta.
bool estrutura_inserir(ESTRUTURA *e, ITEM *item, double similaridade);

// remove o item da "frente" da estrutura (topo da pilha, inicio da fila ou primeiro da lista). em lista_gen e lista_cruz a remocao nao foi implementada e devolve null.
ITEM *estrutura_remover(ESTRUTURA *e);

int estrutura_tamanho(const ESTRUTURA *e);

// percurso na ordem de acesso de cada estrutura (e a ordem em que o resultado e gravado no arquivo): pilha do topo para a base, fila e listas do inicio para o fim, lista_gen grupo por grupo e lista_cruz pelo primeiro campo do cross.
void estrutura_reiniciar(ESTRUTURA *e);
ITEM *estrutura_proximo(ESTRUTURA *e);

#endif
