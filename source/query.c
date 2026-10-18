#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "query.h"

#define MAX_REGRAS 20
#define TAM_TEXTO 64

typedef struct{
    char campo_filter[TAM_TEXTO];
    char operador[3]; // "==", "!=", "<=", ">=", "<", ">" + '\0'
    char valor[TAM_TEXTO];
}FilterRule;

typedef struct{
    char campo_sim[TAM_TEXTO];
    double alvo;
    double tolerancia;
    double peso;
}SimRule;

struct query{
    //filter
    FilterRule filtros[MAX_REGRAS];
    int qtda_filtros;

    //sim
    SimRule sims[MAX_REGRAS];
    int qtda_sims;

    //accept 
    double limiar_accept;

    //order 
    char campo_order[TAM_TEXTO]; 
    char sentido[8];

    //group 
    char campo_group1[TAM_TEXTO];
    char campo_group2[TAM_TEXTO];

    //cross 
    char campo_cross1[TAM_TEXTO];
    char campo_cross2[TAM_TEXTO];
};

// remove espacos e tabulacoes do comeco e do fim do texto (altera o proprio texto)
static void tirar_espacos(char *texto){
    int inicio = 0;
    while(texto[inicio] != '\0' && isspace((unsigned char)texto[inicio])){
        inicio++;
    }
    int fim = strlen(texto);
    while(fim > inicio && isspace((unsigned char)texto[fim - 1])){
        fim--;
    }
    memmove(texto, texto + inicio, fim - inicio);
    texto[fim - inicio] = '\0';
}

// copia o texto para o destino sem estourar o tamanho dele
static void copiar_seguro(char *destino, const char *origem, int tamanho){
    strncpy(destino, origem, tamanho - 1);
    destino[tamanho - 1] = '\0';
}

// pega o proximo pedaco da linha separado por '|', ja sem espacos nas pontas
static char *proximo_pedaco(void){
    char *pedaco = strtok(NULL, "|");
    if(pedaco != NULL){
        tirar_espacos(pedaco);
    }
    return pedaco;
}

// cria e carrega as regras a partir do arquivo de consulta (.txt)
QUERY *query_carregar(const char *caminho_query){
    if(caminho_query == NULL){
        return NULL;
    }
    FILE *fp = fopen(caminho_query, "r"); // abre o arquivo de regras para leitura
    if(fp == NULL){
        return NULL; 
    }
    QUERY *q = (QUERY *)malloc(sizeof(QUERY)); // reserva memoria para guardar as regras
    if(q == NULL){
        fclose(fp);
        return NULL;
    }
    q->qtda_filtros = 0; // evita que leem lixo de memoria, melhor inicializar com zero
    q->qtda_sims = 0;
    q->limiar_accept = 0.0;
    q->campo_order[0] = '\0';
    strcpy(q->sentido, "DESC"); // define a ordenacao decrescente como padrao
    q->campo_group1[0] = '\0';
    q->campo_group2[0] = '\0';
    q->campo_cross1[0] = '\0';
    q->campo_cross2[0] = '\0';

    char linha[1024]; // buffer, um vetor temporario para guardar os caracteres
    while(fgets(linha,sizeof(linha), fp) != NULL){ // varredura para remover o '\r' ou '\n' do final de cada uma das linhas strcspn (string complement span): a string 2 contem o/os caracteres que estou procurando e na string 1 e analisado se esse caractere e encontrado, se sim pode ser substituido por outro, como, nesse caso, o '\0'
        linha[strcspn(linha, "\r\n")] = '\0'; // remove a quebra de linha do comando

        char *cmd = strtok(linha, "|"); // separa o tipo de regra do restante da linha
        if(cmd == NULL){
            continue; // linha em branco
        }
        tirar_espacos(cmd);

        if(strcmp(cmd,"FILTER") == 0){
            char *campo_filt = proximo_pedaco(); // por exemplo "is_future_event"
            char *oper = proximo_pedaco(); // por exemplo '<='
            char *val = proximo_pedaco(); // por exemplo "true"

            // 1) verifica se todos os campos existem e se e menor que o maximo de filtros
            if(campo_filt && oper && val && strlen(oper) <= 2 && q->qtda_filtros < MAX_REGRAS){ // valida e limita os filtros
                // 2) copia os textos para struct na posicao atual
                FilterRule *regra = &q->filtros[q->qtda_filtros];
                copiar_seguro(regra->campo_filter, campo_filt, TAM_TEXTO); // copia o campo com limite
                copiar_seguro(regra->operador, oper, 3); // copia o operador com terminador
                copiar_seguro(regra->valor, val, TAM_TEXTO); // copia o valor do filtro
                q->qtda_filtros++;
            }
        }
        else if(strcmp(cmd,"SIM") == 0){
            char *campo_sim = proximo_pedaco();
            char *alvo_str = proximo_pedaco();
            char *tolerancia_str = proximo_pedaco();
            char *peso_str = proximo_pedaco();

            if(campo_sim && alvo_str && tolerancia_str && peso_str && q->qtda_sims < MAX_REGRAS){
                // necessario para transformar a string em um valor double. utilizacao do atof: que tranforma string em um float.
                SimRule *regra = &q->sims[q->qtda_sims];
                copiar_seguro(regra->campo_sim, campo_sim, TAM_TEXTO);
                regra->alvo = atof(alvo_str); // converte o alvo para numero
                regra->tolerancia = atof(tolerancia_str); // converte a tolerancia para numero
                regra->peso = atof(peso_str); // converte o peso para numero
                q->qtda_sims++;
            }
        }
        else if(strcmp(cmd,"ACCEPT") == 0){
            char *limiar_str = proximo_pedaco();

            if(limiar_str != NULL) {
                q->limiar_accept = atof(limiar_str); // converte o limite de aceitacao
            }
        }
        else if(strcmp(cmd,"ORDER") == 0){
            char *campo_ord = proximo_pedaco();
            char *sent = proximo_pedaco();

            if(campo_ord != NULL){
                copiar_seguro(q->campo_order, campo_ord, TAM_TEXTO);
            }
            if(sent != NULL){
                copiar_seguro(q->sentido, sent, 8); 
            }
        }
        else if(strcmp(cmd,"GROUP") == 0){
            char *g1 = proximo_pedaco();
            char *g2 = proximo_pedaco();

            if(g1 != NULL){
                copiar_seguro(q->campo_group1, g1, TAM_TEXTO);
            }
            if(g2 != NULL){
                copiar_seguro(q->campo_group2, g2, TAM_TEXTO);
            }
        }
        else if(strcmp(cmd,"CROSS") == 0){
            char *c1 = proximo_pedaco();
            char *c2 = proximo_pedaco();

            if(c1 != NULL){
                copiar_seguro(q->campo_cross1, c1, TAM_TEXTO);
            }
            if(c2 != NULL){
                copiar_seguro(q->campo_cross2, c2, TAM_TEXTO);
            }
        }
    }
    fclose(fp); // fecha o arquivo depois de carregar as regras
    return q;
}

// apaga o tad query e libera a memoria
void query_apagar(QUERY **q){
    if(q == NULL || *q == NULL){
        return;
    }
    free(*q); // libera a struct query alocada no malloc
    *q = NULL; // zera o ponteiro original para evitar ponteiro pendente (dangling pointer)
}

// aplica o operador entre dois numeros
static bool comparar_numeros(double valor_item, const char *operador, double valor_regra){
    if(strcmp(operador, "==") == 0) return fabs(valor_item - valor_regra) < 1e-9; //fabs calcula o modulo

    if(strcmp(operador, "!=") == 0) return fabs(valor_item - valor_regra) >= 1e-9;

    if(strcmp(operador, "<=") == 0) return valor_item <= valor_regra;

    if(strcmp(operador, ">=") == 0) return valor_item >= valor_regra;

    if(strcmp(operador, "<") == 0)  return valor_item < valor_regra;

    if(strcmp(operador, ">") == 0)  return valor_item > valor_regra;

    return false; // operador desconhecido nunca aceita
}

// avalia um filter para o item. - campo ausente (vazio) reprova o item - se os dois lados forem numeros (ou true/false), compara como numero - senao compara como texto, e so "==" e "!=" fazem sentido
static bool avaliar_filtro(const FilterRule *regra, const ITEM *item){
    const char *valor_item = item_get_campo(item, regra->campo_filter);
    if(valor_item == NULL){
        return false;
    }

    double numero_item;
    double numero_regra;
    if(item_converter_numero(valor_item, &numero_item) &&
       item_converter_numero(regra->valor, &numero_regra)){
        return comparar_numeros(numero_item, regra->operador, numero_regra);
    }

    if(strcmp(regra->operador, "==") == 0){
        return strcmp(valor_item, regra->valor) == 0;
    }
    if(strcmp(regra->operador, "!=") == 0){
        return strcmp(valor_item, regra->valor) != 0;
    }
    return false;
}

// s(x, q, t) = max(0, 1 - |x - q| / t)
static double similaridade_do_criterio(const SimRule *regra, const ITEM *item){
    double x;
    if(!item_get_campo_numerico(item, regra->campo_sim, &x)){
        return 0.0; // valor ausente nao se parece com o alvo
    }
    if(regra->tolerancia <= 0.0){
        return (x == regra->alvo) ? 1.0 : 0.0; // tolerancia invalida: so aceita igual
    }
    double similaridade = 1.0 - fabs(x - regra->alvo) / regra->tolerancia;
    return similaridade > 0.0 ? similaridade : 0.0;
}

// avalia um item em relacao aos filtes e sims da consulta returna 'true' se o item for aceito e calcula a similaridade obtida
bool query_avaliar_item(QUERY *q, ITEM *item, double *similaridade_calc){
    if(q == NULL || item == NULL){
        return false;
    }

    //avalia todos os filters: um que falhe ja reprova o item
    for(int i = 0; i < q->qtda_filtros; i++){
        if(!avaliar_filtro(&q->filtros[i], item)){
            return false;
        }
    }

    //sem sim, os filtros sao suficientes
    if(q->qtda_sims == 0){
        if(similaridade_calc != NULL){
            *similaridade_calc = 1.0;
        }
        return true;
    }

    //similaridade total = media ponderada dos criterios sim
    double soma_ponderada = 0.0;
    double soma_pesos = 0.0;
    for(int i = 0; i < q->qtda_sims; i++){
        soma_ponderada += q->sims[i].peso * similaridade_do_criterio(&q->sims[i], item);
        soma_pesos += q->sims[i].peso;
    }
    double similaridade_total = 0.0;
    if(soma_pesos > 0.0){
        similaridade_total = soma_ponderada / soma_pesos;
    }
    if(similaridade_calc != NULL){
        *similaridade_calc = similaridade_total;
    }

    //accept: similaridade minima para aceitar (pequena folga por causa do double)
    return similaridade_total >= q->limiar_accept - 1e-12;
}

// campo da ordenacao (vazio se a consulta nao tem order)
const char *query_get_campo_order(const QUERY *q){
    if(q == NULL){
        return "";
    }
    return q->campo_order;
}

// so eh crescente se a consulta pediu asc; sem order o padrao eh desc
bool query_order_decrescente(const QUERY *q){
    if(q == NULL){
        return true;
    }
    return strcmp(q->sentido, "ASC") != 0;
}

const char *query_get_campo_group(const QUERY *q, int nivel){
    if(q == NULL){
        return "";
    }
    return (nivel == 1) ? q->campo_group1 : q->campo_group2;
}

const char *query_get_campo_cross(const QUERY *q, int nivel){
    if(q == NULL){
        return "";
    }
    return (nivel == 1) ? q->campo_cross1 : q->campo_cross2;
}
