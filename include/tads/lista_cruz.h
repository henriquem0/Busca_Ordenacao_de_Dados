#ifndef LISTA_CRUZ_H
#define LISTA_CRUZ_H

#include <stdbool.h>
#include "item.h"

typedef struct lista_cruz LISTA_CRUZ;

// a lista cruzada tem duas organizacoes ao mesmo tempo (eixos). cada item existe uma unica vez;

typedef enum {
    LISTA_CRUZ_EIXO_A = 0, // eixo a
    LISTA_CRUZ_EIXO_B = 1 // eixo b
} EixoCruz;


LISTA_CRUZ *lista_cruz_criar(void); // cria a lista cruzada

void lista_cruz_apagar(LISTA_CRUZ **lista); // destroi a lista e libera a memoria (incluindo os itens)


void lista_cruz_apagar_sem_itens(LISTA_CRUZ **lista);// destroi so a lista mas os itens continuam existindo (usado na estrutura de resultado)

// insere o item uma unica vez, ligando ele na cadeia da chave_a (eixo a) e na cadeia da chave_b (eixo b). as chaves sao textos, entao funciona com qualquer campo de qualquer dataset (numeros devem ser convertidos before).
bool lista_cruz_inserir(LISTA_CRUZ *lista, ITEM *item, const char *chave_a, const char *chave_b);


ITEM *lista_cruz_buscar(const LISTA_CRUZ *lista, const char *identificador);// busca o primeiro item com o identificador informado (o item continua na lista)

int lista_cruz_tamanho(const LISTA_CRUZ *lista); // retorna a quantidade de itens (cada item conta uma vez, nao uma por eixo)

// percurso por um dos eixos: devolve os itens cadeia por cadeia, na ordem em que as chaves apareceram pela primeira vez. cada eixo tem seu proprio cursor.
void lista_cruz_reiniciar(LISTA_CRUZ *lista, EixoCruz eixo);

ITEM *lista_cruz_proximo(LISTA_CRUZ *lista, EixoCruz eixo);

#endif
