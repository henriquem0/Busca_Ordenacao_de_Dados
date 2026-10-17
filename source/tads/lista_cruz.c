#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tads/lista_cruz.h"

#define NUM_EIXOS 2

typedef struct no_cruz NO_CRUZ;
typedef struct cabeca_cadeia CABECA_CADEIA;

// No unico do item: tem um ponteiro "proximo" para cada eixo.
// proximo[EIXO_A] liga os itens de mesma chave A, proximo[EIXO_B] os de mesma chave B.
struct no_cruz{
    ITEM *item;
    NO_CRUZ *proximo[NUM_EIXOS];
};

// Cabeca de uma cadeia: guarda a chave (ex: "SAFE" ou "2024") e os extremos da cadeia
struct cabeca_cadeia{
    char *chave;
    NO_CRUZ *primeiro;
    NO_CRUZ *ultimo;
    CABECA_CADEIA *proxima; // proxima cadeia do mesmo eixo
};

struct lista_cruz{
    CABECA_CADEIA *primeira[NUM_EIXOS]; // primeira cadeia de cada eixo
    CABECA_CADEIA *ultima[NUM_EIXOS];   // ultima cadeia de cada eixo
    int tamanho;
    // cursor do percurso de cada eixo
    CABECA_CADEIA *cabeca_percurso[NUM_EIXOS];
    NO_CRUZ *no_percurso[NUM_EIXOS];
};

// Copia um texto para a memoria dinamica
static char *copiar_texto(const char *texto){
    char *copia = (char *)malloc(strlen(texto) + 1);
    if(copia != NULL){
        strcpy(copia, texto);
    }
    return copia;
}

// Compara o identificador do item com o texto procurado
static bool mesmo_identificador(const ITEM *item, const char *identificador){
    const char *identificador_item = item_get_identificador(item);
    return identificador_item != NULL && strcmp(identificador_item, identificador) == 0;
}

// Confere se o eixo recebido e valido
static bool eixo_valido(EixoCruz eixo){
    return eixo == LISTA_CRUZ_EIXO_A || eixo == LISTA_CRUZ_EIXO_B;
}

// Procura a cadeia da chave no eixo. Se ainda nao existe, cria no fim.
static CABECA_CADEIA *buscar_ou_criar_cadeia(LISTA_CRUZ *lista, int eixo, const char *chave){
    CABECA_CADEIA *cabeca = lista->primeira[eixo];
    while(cabeca != NULL && strcmp(cabeca->chave, chave) != 0){
        cabeca = cabeca->proxima;
    }
    if(cabeca != NULL){
        return cabeca;
    }

    cabeca = (CABECA_CADEIA *)malloc(sizeof(CABECA_CADEIA));
    if(cabeca == NULL){
        return NULL;
    }
    cabeca->chave = copiar_texto(chave);
    if(cabeca->chave == NULL){
        free(cabeca);
        return NULL;
    }
    cabeca->primeiro = NULL;
    cabeca->ultimo = NULL;
    cabeca->proxima = NULL;

    if(lista->ultima[eixo] == NULL){
        lista->primeira[eixo] = cabeca;
    }
    else{
        lista->ultima[eixo]->proxima = cabeca;
    }
    lista->ultima[eixo] = cabeca;
    return cabeca;
}

// Liga o no no fim de uma cadeia, usando o ponteiro proximo do eixo
static void ligar_na_cadeia(CABECA_CADEIA *cabeca, NO_CRUZ *no, int eixo){
    if(cabeca->ultimo == NULL){
        cabeca->primeiro = no;
    }
    else{
        cabeca->ultimo->proximo[eixo] = no;
    }
    cabeca->ultimo = no;
}

// Cria a lista vazia
LISTA_CRUZ *lista_cruz_criar(void){
    LISTA_CRUZ *lista = (LISTA_CRUZ *)malloc(sizeof(LISTA_CRUZ));
    if(lista == NULL){
        return NULL;
    }
    for(int eixo = 0; eixo < NUM_EIXOS; eixo++){
        lista->primeira[eixo] = NULL;
        lista->ultima[eixo] = NULL;
        lista->cabeca_percurso[eixo] = NULL;
        lista->no_percurso[eixo] = NULL;
    }
    lista->tamanho = 0;
    return lista;
}

// Libera a lista. Todo no esta em exatamente uma cadeia do eixo A, entao
// percorremos so esse eixo para liberar os nos (e os itens, se pedido).
// Depois liberamos as cabecas das cadeias dos dois eixos.
static void lista_cruz_liberar(LISTA_CRUZ **lista, bool apagar_itens){
    if(lista == NULL || *lista == NULL){
        return;
    }
    CABECA_CADEIA *cabeca = (*lista)->primeira[LISTA_CRUZ_EIXO_A];
    while(cabeca != NULL){
        NO_CRUZ *no = cabeca->primeiro;
        while(no != NULL){
            NO_CRUZ *proximo_no = no->proximo[LISTA_CRUZ_EIXO_A];
            if(apagar_itens){
                item_apagar(&(no->item));
            }
            free(no);
            no = proximo_no;
        }
        cabeca = cabeca->proxima;
    }

    for(int eixo = 0; eixo < NUM_EIXOS; eixo++){
        cabeca = (*lista)->primeira[eixo];
        while(cabeca != NULL){
            CABECA_CADEIA *proxima = cabeca->proxima;
            free(cabeca->chave);
            free(cabeca);
            cabeca = proxima;
        }
    }
    free(*lista);
    *lista = NULL;
}

// Destroi a lista e libera a memoria
void lista_cruz_apagar(LISTA_CRUZ **lista){
    lista_cruz_liberar(lista, true);
}

// Destroi so a lista, os itens continuam existindo
void lista_cruz_apagar_sem_itens(LISTA_CRUZ **lista){
    lista_cruz_liberar(lista, false);
}

// Insere o item uma unica vez, ligado nas duas cadeias.
// Se faltar memoria para criar uma cadeia, a insercao falha e o item nao entra
// (pode sobrar uma cadeia vazia, que nao atrapalha o percurso).
bool lista_cruz_inserir(LISTA_CRUZ *lista, ITEM *item,
                        const char *chave_a, const char *chave_b){
    if(lista == NULL || item == NULL || chave_a == NULL || chave_b == NULL){
        return false;
    }
    NO_CRUZ *novo = (NO_CRUZ *)malloc(sizeof(NO_CRUZ));
    if(novo == NULL){
        return false;
    }
    novo->item = item;
    novo->proximo[LISTA_CRUZ_EIXO_A] = NULL;
    novo->proximo[LISTA_CRUZ_EIXO_B] = NULL;

    CABECA_CADEIA *cadeia_a = buscar_ou_criar_cadeia(lista, LISTA_CRUZ_EIXO_A, chave_a);
    CABECA_CADEIA *cadeia_b = NULL;
    if(cadeia_a != NULL){
        cadeia_b = buscar_ou_criar_cadeia(lista, LISTA_CRUZ_EIXO_B, chave_b);
    }
    if(cadeia_a == NULL || cadeia_b == NULL){
        free(novo);
        return false;
    }

    ligar_na_cadeia(cadeia_a, novo, LISTA_CRUZ_EIXO_A);
    ligar_na_cadeia(cadeia_b, novo, LISTA_CRUZ_EIXO_B);
    lista->tamanho++;
    return true;
}

// Busca por identificador percorrendo as cadeias do eixo A, custo O(n)
ITEM *lista_cruz_buscar(const LISTA_CRUZ *lista, const char *identificador){
    if(lista == NULL || identificador == NULL){
        return NULL;
    }
    for(CABECA_CADEIA *cabeca = lista->primeira[LISTA_CRUZ_EIXO_A]; cabeca != NULL;
        cabeca = cabeca->proxima){
        for(NO_CRUZ *no = cabeca->primeiro; no != NULL; no = no->proximo[LISTA_CRUZ_EIXO_A]){
            if(mesmo_identificador(no->item, identificador)){
                return no->item;
            }
        }
    }
    return NULL;
}

// Retorna a quantidade de itens
int lista_cruz_tamanho(const LISTA_CRUZ *lista){
    if(lista == NULL){
        return 0;
    }
    return lista->tamanho;
}

// Coloca o cursor do eixo no primeiro item da primeira cadeia
void lista_cruz_reiniciar(LISTA_CRUZ *lista, EixoCruz eixo){
    if(lista == NULL || !eixo_valido(eixo)){
        return;
    }
    lista->cabeca_percurso[eixo] = lista->primeira[eixo];
    lista->no_percurso[eixo] = NULL;
    if(lista->cabeca_percurso[eixo] != NULL){
        lista->no_percurso[eixo] = lista->cabeca_percurso[eixo]->primeiro;
    }
}

// Devolve o proximo item do eixo. Quando uma cadeia acaba, passa para a proxima.
ITEM *lista_cruz_proximo(LISTA_CRUZ *lista, EixoCruz eixo){
    if(lista == NULL || !eixo_valido(eixo)){
        return NULL;
    }
    while(true){
        if(lista->no_percurso[eixo] != NULL){
            ITEM *item = lista->no_percurso[eixo]->item;
            lista->no_percurso[eixo] = lista->no_percurso[eixo]->proximo[eixo];
            return item;
        }
        if(lista->cabeca_percurso[eixo] == NULL){
            return NULL; // percurso terminou
        }
        lista->cabeca_percurso[eixo] = lista->cabeca_percurso[eixo]->proxima;
        if(lista->cabeca_percurso[eixo] == NULL){
            return NULL;
        }
        lista->no_percurso[eixo] = lista->cabeca_percurso[eixo]->primeiro;
    }
}
