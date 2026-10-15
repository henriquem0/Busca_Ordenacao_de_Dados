#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "query.h"

typedef struct{
    char campo_filter[64];
    char operador[3]; // "==", "!=", "<=", ">=", "<;", ">;" + '\0'
    char valor[64];
}FilterRule;

typedef struct{
    char campo_sim[64];
    double alvo;
    double tolerancia;
    double peso;
}SimRule;

struct query{
    //------- FILTER --------
    FilterRule filtros[20];
    int qtda_filtros;

    //------- SIM --------
    SimRule sims[20];
    int qtda_sims;

    //------- ACCEPT --------
    double limiar_accept;

    //------- ORDER --------
    char campo_order[64]; 
    char sentido[8];

    //------- GROUP --------
    char campo_group1[64];
    char campo_group2[64];

    //------- CROSS --------
    char campo_cross1[64];
    char campo_cross2[64];
    
};
// Cria e carrega as regras a partir do arquivo de consulta (.txt)
QUERY *query_carregar(const char *caminho_query){
    FILE *fp = fopen(caminho_query, "r");
    if(fp == NULL){
        return NULL; 
    }
    QUERY *q = (QUERY *)malloc(sizeof(QUERY));
    if(q == NULL){
        fclose(fp);
        return NULL;
    }
    q->qtda_filtros = 0; //evita que leem lixo de memoria, melhor inicializar com zero
    q->qtda_sims = 0;
    q->limiar_accept = 0.0;

    char linha[1024]; //buffer, um vetor temporario para guardar os caracteres
    while(fgets(linha,sizeof(linha), fp) != NULL){
        //Varredura para remover o '\r' ou '\n' do final de cada uma das linhas
        //strcspn (string complement span): a string 2 contem o/os caracteres que estou procurando
        //e na string 1 e analisado se esse caractere e encontrado, se sim pode ser substituido por outro,
        //como, nesse caso, o '\0'
        linha[strcspn(linha, "\r\n")] = '\0';

        char *cmd = strtok(linha, "|");
        if(strcmp(cmd,"FILTER") == 0){
            char *campo_filt = strtok(NULL, "|"); //por exemplo "is_future_event"
            char *oper = strtok(NULL, "|"); //por exemplo '<='
            char *val == strtok(NULL, "|"); //por exemplo "true"

            // 1) Verifica se todos os campos existem e se e menor que 20 o qtda de filtros
            if(campo_filt && oper && val && q->qtda_filtros < 20){
                // 2) Copia os textos para struct na posicao atual  
                strcpy(q->filtros[q->qtda_filtros].campo_filter, campo_filt); 
                strcpy(q->filtros[q->qtda_filtros].operador, oper); 
                strcpy(q->filtros[q->qtda_filtros].valor, val);
                q->qtda_filtros++;
            }
        }
        else if(strcmp(cmd,"SIM") == 0){
            char *campo_sim = strtok(NULL, "|");
            char *alvo_str = strtok(NULL, "|");
            char *tolerancia_str = strtok(NULL, "|");
            char *peso_str = strtok(NULL, "|");

            if(campo_sim && alvo_str && tolerancia_str && peso_str && q->qtda_sims < 20){
            
                //Necessario para transformar a string em um valor double.
                //Utilizacao do atof: que tranforma string em um float.
                strcpy(q->sims[q->qtda_sims].campo_sim, campo_sim);
                q->sims[q->qtda_sims].alvo = atof(alvo_str);
                q->sims[q->qtda_sims].tolerancia = atof(tolerancia_str);
                q->sims[q->qtda_sims].peso = atof(peso_str);
                q->qtda_sims++;
            }
        }
        else if(strcmp(cmd,"ACCEPT") == 0){
            char *limiar_str = strtok(NULL, "|");

            if(limiar_str != NULL) {
                q->limiar_accept = atof(limiar_str);
            }
        }
        else if(strcmp(cmd,"ORDER") == 0){
            char *campo_ord = strtok(NULL, "|");
            char *sent = strtok(NULL, "|");

            if(campo_ord != NULL && sent != NULL){
                strcpy(q->campo_order, campo_ord);
                strcpy(q->sentido, sent); 
            }
        }
        else if(strcmp(cmd,"GROUP") == 0){
            char *g1 = strtok(NULL, "|");
            char *g2 = strtok(NULL, "|");

            if(g1 != NULL){
                strcpy(q->campo_group1, g1);
            }
            if(g2 != NULL){
                strcpy(q->campo_group2, g2);
            }
        }
        else if(strcmp(cmd,"CROSS") == 0){
            char *g1 = strtok(NULL, "|");
            char *g2 = strtok(NULL, "|");

            if(g1 != NULL){
                strcpy(q->campo_cross1, g1);
            }
            if(g2 != NULL){
                strcpy(q->campo_cross2, g2);
            }
        }

    }
    fclose(fp);
    return q;
}

// Apaga o TAD Query e libera a memoria
void query_apagar(QUERY **q){
    if(q == NULL || *q == NULL){
        return
    }
    free(*q); // Libera a struct query alocada no malloc
    *q = NULL; // Zera o ponteiro original para evitar ponteiro pendente (dangling pointer)
}

// Avalia um ITEM em relacao aos FILTES e SIMs da consulta
// Returna 'true' se o item for aceito e calcula a similaridade obtida
bool query_avaliar_item(QUERY *q, ITEM *item, double *similaridade_calc){
    if(q == NULL || item == NULL){
        return false;
    }

    // 1) Avalia todos os FILTERs
    for(int i = 0; i < q->qtda_filtros; i++){
        FiltroRule *f = 
    }
}