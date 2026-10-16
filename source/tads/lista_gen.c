#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tads/lista_gen.h"

typedef struct no_gen NO_GEN;
typedef struct subgrupo SUBGRUPO;
typedef struct grupo GRUPO;

// Nivel 3: no que guarda o item
struct no_gen{
    ITEM *item;
    NO_GEN *proximo;
};

// Nivel 2: subgrupo, com a sua propria lista de itens
struct subgrupo{
    char *nome;
    NO_GEN *primeiro;
    NO_GEN *ultimo;
    SUBGRUPO *proximo;
};

// Nivel 1: grupo, com a sua propria lista de subgrupos
struct grupo{
    char *nome;
    SUBGRUPO *primeiro_subgrupo;
    SUBGRUPO *ultimo_subgrupo;
    GRUPO *proximo;
};

struct lista_gen{
    GRUPO *primeiro_grupo;
    GRUPO *ultimo_grupo;
    int tamanho;
    // cursor do percurso: em qual grupo, subgrupo e no estamos
    GRUPO *grupo_percurso;
    SUBGRUPO *subgrupo_percurso;
    NO_GEN *no_percurso;
};

// Copia um texto para a memoria dinamica
static char *copiar_texto(const char *texto){
    char *copia = (char *)malloc(strlen(texto) + 1);
    if(copia != NULL){
        strcpy(copia, texto);
    }
    return copia;
}

// Compara a designacao do item com o texto procurado
static bool mesma_designacao(const ITEM *item, const char *designacao){
    const char *designacao_item = item_get_designation(item);
    return designacao_item != NULL && strcmp(designacao_item, designacao) == 0;
}

// Procura um grupo pelo nome. Devolve NULL se ele ainda nao existe.
static GRUPO *buscar_grupo(const LISTA_GEN *lista, const char *nome){
    GRUPO *grupo = lista->primeiro_grupo;
    while(grupo != NULL && strcmp(grupo->nome, nome) != 0){
        grupo = grupo->proximo;
    }
    return grupo;
}

// Cria um grupo novo no fim da lista de grupos
static GRUPO *criar_grupo(LISTA_GEN *lista, const char *nome){
    GRUPO *grupo = (GRUPO *)malloc(sizeof(GRUPO));
    if(grupo == NULL){
        return NULL;
    }
    grupo->nome = copiar_texto(nome);
    if(grupo->nome == NULL){
        free(grupo);
        return NULL;
    }
    grupo->primeiro_subgrupo = NULL;
    grupo->ultimo_subgrupo = NULL;
    grupo->proximo = NULL;

    if(lista->ultimo_grupo == NULL){
        lista->primeiro_grupo = grupo;
    }
    else{
        lista->ultimo_grupo->proximo = grupo;
    }
    lista->ultimo_grupo = grupo;
    return grupo;
}

// Procura um subgrupo pelo nome dentro de um grupo
static SUBGRUPO *buscar_subgrupo(const GRUPO *grupo, const char *nome){
    SUBGRUPO *subgrupo = grupo->primeiro_subgrupo;
    while(subgrupo != NULL && strcmp(subgrupo->nome, nome) != 0){
        subgrupo = subgrupo->proximo;
    }
    return subgrupo;
}

// Cria um subgrupo novo no fim da lista de subgrupos do grupo
static SUBGRUPO *criar_subgrupo(GRUPO *grupo, const char *nome){
    SUBGRUPO *subgrupo = (SUBGRUPO *)malloc(sizeof(SUBGRUPO));
    if(subgrupo == NULL){
        return NULL;
    }
    subgrupo->nome = copiar_texto(nome);
    if(subgrupo->nome == NULL){
        free(subgrupo);
        return NULL;
    }
    subgrupo->primeiro = NULL;
    subgrupo->ultimo = NULL;
    subgrupo->proximo = NULL;

    if(grupo->ultimo_subgrupo == NULL){
        grupo->primeiro_subgrupo = subgrupo;
    }
    else{
        grupo->ultimo_subgrupo->proximo = subgrupo;
    }
    grupo->ultimo_subgrupo = subgrupo;
    return subgrupo;
}

// Cria a lista vazia
LISTA_GEN *lista_gen_criar(void){
    LISTA_GEN *lista = (LISTA_GEN *)malloc(sizeof(LISTA_GEN));
    if(lista == NULL){
        return NULL;
    }
    lista->primeiro_grupo = NULL;
    lista->ultimo_grupo = NULL;
    lista->tamanho = 0;
    lista->grupo_percurso = NULL;
    lista->subgrupo_percurso = NULL;
    lista->no_percurso = NULL;
    return lista;
}

// Libera a lista inteira: itens (se pedido), nos, subgrupos, grupos e a lista
static void lista_gen_liberar(LISTA_GEN **lista, bool apagar_itens){
    if(lista == NULL || *lista == NULL){
        return;
    }
    GRUPO *grupo = (*lista)->primeiro_grupo;
    while(grupo != NULL){
        SUBGRUPO *subgrupo = grupo->primeiro_subgrupo;
        while(subgrupo != NULL){
            NO_GEN *no = subgrupo->primeiro;
            while(no != NULL){
                NO_GEN *proximo_no = no->proximo;
                if(apagar_itens){
                    item_apagar(&(no->item));
                }
                free(no);
                no = proximo_no;
            }
            SUBGRUPO *proximo_subgrupo = subgrupo->proximo;
            free(subgrupo->nome);
            free(subgrupo);
            subgrupo = proximo_subgrupo;
        }
        GRUPO *proximo_grupo = grupo->proximo;
        free(grupo->nome);
        free(grupo);
        grupo = proximo_grupo;
    }
    free(*lista);
    *lista = NULL;
}

// Destroi a lista e libera a memoria
void lista_gen_apagar(LISTA_GEN **lista){
    lista_gen_liberar(lista, true);
}

// Destroi so a lista, os itens continuam existindo
void lista_gen_apagar_sem_itens(LISTA_GEN **lista){
    lista_gen_liberar(lista, false);
}

// Insere o item no grupo/subgrupo (criando os que nao existirem).
// Custo: O(g + s) para achar o grupo e o subgrupo, mais O(1) para ligar o no.
// Se faltar memoria para criar um grupo ou subgrupo, a insercao falha e o item
// nao entra (pode sobrar um grupo vazio, que nao atrapalha o percurso).
bool lista_gen_inserir(LISTA_GEN *lista, ITEM *item,
                       const char *nome_grupo, const char *nome_subgrupo){
    if(lista == NULL || item == NULL || nome_grupo == NULL || nome_subgrupo == NULL){
        return false;
    }
    NO_GEN *novo = (NO_GEN *)malloc(sizeof(NO_GEN));
    if(novo == NULL){
        return false;
    }
    novo->item = item;
    novo->proximo = NULL;

    GRUPO *grupo = buscar_grupo(lista, nome_grupo);
    if(grupo == NULL){
        grupo = criar_grupo(lista, nome_grupo);
    }
    SUBGRUPO *subgrupo = NULL;
    if(grupo != NULL){
        subgrupo = buscar_subgrupo(grupo, nome_subgrupo);
        if(subgrupo == NULL){
            subgrupo = criar_subgrupo(grupo, nome_subgrupo);
        }
    }
    if(subgrupo == NULL){
        free(novo);
        return false;
    }

    if(subgrupo->ultimo == NULL){
        subgrupo->primeiro = novo;
    }
    else{
        subgrupo->ultimo->proximo = novo;
    }
    subgrupo->ultimo = novo;
    lista->tamanho++;
    return true;
}

// Busca por designacao percorrendo todos os grupos e subgrupos, custo O(n)
ITEM *lista_gen_buscar(const LISTA_GEN *lista, const char *designacao){
    if(lista == NULL || designacao == NULL){
        return NULL;
    }
    for(GRUPO *grupo = lista->primeiro_grupo; grupo != NULL; grupo = grupo->proximo){
        for(SUBGRUPO *subgrupo = grupo->primeiro_subgrupo; subgrupo != NULL;
            subgrupo = subgrupo->proximo){
            for(NO_GEN *no = subgrupo->primeiro; no != NULL; no = no->proximo){
                if(mesma_designacao(no->item, designacao)){
                    return no->item;
                }
            }
        }
    }
    return NULL;
}

// Retorna a quantidade total de itens
int lista_gen_tamanho(const LISTA_GEN *lista){
    if(lista == NULL){
        return 0;
    }
    return lista->tamanho;
}

// Coloca o cursor no primeiro item do primeiro subgrupo do primeiro grupo
void lista_gen_reiniciar(LISTA_GEN *lista){
    if(lista == NULL){
        return;
    }
    lista->grupo_percurso = lista->primeiro_grupo;
    lista->subgrupo_percurso = NULL;
    lista->no_percurso = NULL;
    if(lista->grupo_percurso != NULL){
        lista->subgrupo_percurso = lista->grupo_percurso->primeiro_subgrupo;
        if(lista->subgrupo_percurso != NULL){
            lista->no_percurso = lista->subgrupo_percurso->primeiro;
        }
    }
}

// Devolve o proximo item do percurso. Quando os nos de um subgrupo acabam,
// passa para o proximo subgrupo; quando os subgrupos acabam, para o proximo grupo.
ITEM *lista_gen_proximo(LISTA_GEN *lista){
    if(lista == NULL){
        return NULL;
    }
    while(true){
        if(lista->no_percurso != NULL){
            ITEM *item = lista->no_percurso->item;
            lista->no_percurso = lista->no_percurso->proximo;
            return item;
        }
        if(lista->grupo_percurso == NULL){
            return NULL; // percurso terminou
        }
        // acabaram os nos: avanca para o proximo subgrupo
        if(lista->subgrupo_percurso != NULL){
            lista->subgrupo_percurso = lista->subgrupo_percurso->proximo;
        }
        if(lista->subgrupo_percurso == NULL){
            // acabaram os subgrupos: avanca para o proximo grupo
            lista->grupo_percurso = lista->grupo_percurso->proximo;
            if(lista->grupo_percurso == NULL){
                return NULL;
            }
            lista->subgrupo_percurso = lista->grupo_percurso->primeiro_subgrupo;
        }
        lista->no_percurso = NULL;
        if(lista->subgrupo_percurso != NULL){
            lista->no_percurso = lista->subgrupo_percurso->primeiro;
        }
    }
}
