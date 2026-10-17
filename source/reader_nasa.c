#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "reader_nasa.h"
#include "item.h"

/* Le uma linha inteira do arquivo, qualquer que seja o tamanho dela.
   Devolve NULL quando o arquivo acabou. Quem chama deve dar free na linha. */
static char *ler_linha(FILE *fp){
    int capacidade = 512;
    int tamanho = 0;
    char *linha = (char *)malloc(capacidade);
    if(linha == NULL){
        return NULL;
    }

    while(fgets(linha + tamanho, capacidade - tamanho, fp) != NULL){
        tamanho += strlen(linha + tamanho);
        if(linha[tamanho - 1] == '\n'){
            return linha; // linha completa
        }
        // a linha nao coube no buffer: dobra o tamanho e continua lendo
        capacidade *= 2;
        char *aux = (char *)realloc(linha, capacidade);
        if(aux == NULL){
            free(linha);
            return NULL;
        }
        linha = aux;
    }

    if(tamanho > 0){
        return linha; // ultima linha do arquivo, sem '\n' no final
    }
    free(linha);
    return NULL;
}

/*
   - caminho_csv: o caminho do arquivo no disco, o const garante que nao sera alterado
   - limit: o limite de registros a serem lidos definido pelo (--limit)
   - base: estrutura que recebe os itens
*/
int reader_nasa_carregar(const char *caminho_csv, int limit, ESTRUTURA *base){
    if(caminho_csv == NULL || base == NULL){
        return -1;
    }
    // "rb" para ler os bytes do jeito que estao no arquivo (o fim de linha tambem)
    FILE *fp = fopen(caminho_csv, "rb");
    // Verifica se houve erro na abertura
    if(fp == NULL){
        return -1;
    }

    // Primeira linha: nomes das colunas. Nao e um registro.
    char *cabecalho = ler_linha(fp);
    if(cabecalho == NULL){
        fclose(fp);
        return -1;
    }
    item_definir_cabecalho(cabecalho);
    free(cabecalho);

    int qtda = 0;
    char *linha;
    while((linha = ler_linha(fp)) != NULL){
        // Se o usuario pediu um limite (limit > 0) e nos ja atingimos esse limite:
        if(limit > 0 && qtda >= limit){
            free(linha);
            break; 
        }
        // Chama item_criar(linha) e guarda direto na estrutura base
        ITEM *item = item_criar(linha);
        free(linha);
        if(item == NULL){
            continue; // linha em branco
        }
        if(!estrutura_inserir(base, item, 0.0)){
            item_apagar(&item);
            fclose(fp);
            return -1; // sem memoria
        }
        qtda++;
    }

    fclose(fp);
    return qtda;
}
