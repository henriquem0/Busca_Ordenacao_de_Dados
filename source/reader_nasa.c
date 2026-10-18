#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "reader_nasa.h"
#include "item.h"

// le uma linha inteira do arquivo, qualquer que seja o tamanho dela. devolve null quando o arquivo acabou. quem chama deve dar free na linha.
static char *ler_linha(FILE *fp){
    // comeca com um buffer pequeno que pode crescer conforme a linha
    int capacidade = 512;
    int tamanho = 0;
    char *linha = (char *)malloc(capacidade); // reserva memoria para os primeiros caracteres


    if(linha == NULL){
        return NULL;
    }


    // le a linha caractere por caractere ate encontrar um '\n' ou o fim do arquivo
    while(fgets(linha + tamanho, capacidade - tamanho, fp) != NULL){
        // soma quantos caracteres foram lidos nesta parte da linha
        tamanho += strlen(linha + tamanho);

        if(linha[tamanho - 1] == '\n'){
            return linha; // linha completa
        }



        // a linha nao coube no buffer: dobra o tamanho e continua lendo
        capacidade *= 2; // dobra o espaco para tentar ler o restante da linha
        char *aux = (char *)realloc(linha, capacidade); // aumenta o buffer sem perder o texto ja lido

        if(aux == NULL){
            free(linha);
            return NULL;
        }
        linha = aux; // atualiza o ponteiro para o novo buffer
    }

    if(tamanho > 0){
        return linha; // ultima linha do arquivo, sem '\n' no final
    }
    free(linha);
    return NULL;
}

//caminho_csv: o caminho do arquivo no disco, o const garante que nao sera alterado - limit: o limite de registros a serem lidos definido pelo (--limit) - base: estrutura que recebe os itens
int reader_nasa_carregar(const char *caminho_csv, int limit, ESTRUTURA *base){
    if(caminho_csv == NULL || base == NULL){
        return -1;
    }
    // "rb" para ler os bytes do jeito que estao no arquivo (o fim de linha tambem)
    FILE *fp = fopen(caminho_csv, "rb"); // abre o csv em modo binario para preservar as quebras de linha
    // verifica se houve erro na abertura
    if(fp == NULL){
        return -1;
    }



    // primeira linha: nomes das colunas. nao e um registro.
    char *cabecalho = ler_linha(fp); // le a primeira linha com os nomes das colunas
    if(cabecalho == NULL){
        fclose(fp);
        return -1;
    }
    item_definir_cabecalho(cabecalho); // separa e guarda os nomes usados pelos itens
    free(cabecalho);

    int qtda = 0;
    char *linha;



    while((linha = ler_linha(fp)) != NULL){ // le uma linha por vez ate o fim do arquivo
        // se o usuario pediu um limite (limit > 0) e nos ja atingimos esse limite:
        if(limit > 0 && qtda >= limit){
            free(linha);
            break; 
        }


        // chama item_criar(linha) e guarda direto na estrutura base
        ITEM *item = item_criar(linha); // transforma a linha csv em um item
        free(linha);



        if(item == NULL){
            continue; // linha em branco
        }



        if(!estrutura_inserir(base, item, 0.0)){ // coloca o item na estrutura escolhida
            item_apagar(&item);
            fclose(fp);
            return -1; // sem memoria
        }
        qtda++;
    }

    fclose(fp);
    return qtda;
}
