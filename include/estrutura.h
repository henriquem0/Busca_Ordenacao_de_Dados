#ifndef ESTRUTURA_H
#define ESTRUTURA_H

#include <stdbool.h> 
#include "item.h"

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

ESTRUTURA *estrutura_criar(TipoEstrutura tipo);
void estrutura_apagar(ESTRUTURA **e);
bool estrutura_inserir(ESTRUTURA *e, ITEM *item);
ITEM *estrutura_remover(ESTRUTURA *e);

#endif