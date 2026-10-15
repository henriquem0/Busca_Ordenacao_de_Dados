#include <stdio.h> 
#include <stdlib.h>
#include "reader_nasa.h"
#include "item.h"

/*
   - caminho_csv: o caminho do arquivo no disco, o const garante que nao sera alterado
   - limit: o limite de registros a serem lidos definido pelo (--limit)
   - total_lidos: o total de asteroides que foram passados para serem lidos, que pode mudar
*/
ITEM **reader_nasa_csv(const char *caminho_csv, int limit, int *total_lidos){
    FILE *fp = fopen(caminho_csv, "r");
    // Verifica se houve erro na abertura
    if(fp == NULL){
        if(total_lidos != NULL){
            *total_lidos = 0;
        }
        return NULL;
    }

    char linha[1024];
    int qtda = 0;
    int capacidade = 2;
    ITEM **vetor = (ITEM **)malloc(capacidade*sizeof(ITEM *));

    // Le a primeira linha e descarta 
    fgets(linha, sizeof(linha), fp);

    while(fgets(linha, sizeof(linha), fp) != NULL){
        // 1. Processa a linha lida 
        // Se o usuario pediu um limite (limit > 0) e nos ja atingimos esse limite:
        if(limit > 0 && qtda >= limit){
            break; 
        }
        // 2. Chama item_criar(linha) 
        ITEM *item = item_criar(linha);
        if(item != NULL){

             // 3. Guarda no vetor
            if(qtda >= capacidade){
                capacidade *= 2;
                vetor = (ITEM **)realloc(vetor, capacidade *sizeof(ITEM *));
            }
            vetor[qtda] = item;
            qtda++;  
        }
       
    }
    
    fclose(fp);
    *total_lidos = qtda;
    return vetor;
}

void reader_nasa_desalocar(ITEM ***vetor, int total_lidos){
    if(vetor == NULL || *vetor == NULL){
        return;
    }
    for(int i = 0; i < total_lidos; i++){
        item_apagar(&((*vetor)[i]));
        // Usa o destrutor do TAD ITEM para cada asteroide
    }

    free(*vetor); // Libera o vetor de ponteiros
    *vetor = NULL;
}