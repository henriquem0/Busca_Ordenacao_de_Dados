#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#include "cli.h"
#include "item.h"
#include "query.h"
#include "estrutura.h"
#include "reader_nasa.h"

// Tempo atual em milissegundos (relogio que nao volta nem pula)
static double agora_ms(void){
    struct timespec tempo;
    clock_gettime(CLOCK_MONOTONIC, &tempo);
    return tempo.tv_sec * 1000.0 + tempo.tv_nsec / 1000000.0;
}

// Confere se todos os argumentos obrigatorios foram informados
static bool argumentos_completos(const ConfigCLI *config){
    return config->reader != NULL && config->imput_path != NULL &&
           config->base_type != NULL && config->result_type != NULL &&
           config->query_path != NULL && config->output_path != NULL;
}

// Libera tudo o que o programa alocou (pode ser chamada mesmo com partes ainda NULL)
static void liberar_recursos(ESTRUTURA **resultado, ESTRUTURA **base, QUERY **query,
                             ITEM **aceitos, double *similaridades){
    estrutura_apagar(resultado); // so a estrutura: os itens pertencem a base
    estrutura_apagar(base);      // apaga os itens
    query_apagar(query);
    item_liberar_cabecalho();
    free(aceitos);
    free(similaridades);
}

// Mostra o erro no formato esperado e devolve o codigo de saida
static int terminar_com_erro(const char *mensagem){
    printf("STATUS=ERROR\n");
    fprintf(stderr, "Erro: %s\n", mensagem);
    return 1;
}

int main(int argc, char *argv[]){
    //Processa as flags da linha de comando
    ConfigCLI config = cli_parse(argc, argv);
    if(!argumentos_completos(&config)){
        return terminar_com_erro("faltou algum argumento obrigatorio (--reader, --input, --base, --result, --query, --output)");
    }
    // --top-k e --limit nao informados (ou negativos) valem 0: sem limite
    if(config.top_k < 0) config.top_k = 0;
    if(config.limit < 0) config.limit = 0;

    //Variaveis de contagem     
    int total_lidos = 0;
    int matched_records = 0;
    int output_records = 0;

    //Variaveis de medicao de tempo (milissegundos)
    double inicio;
    double load_ms = 0.0;
    double search_ms = 0.0;
    double result_insert_ms = 0.0;
    double write_ms = 0.0;

    // ====== PREPARACAO: tipos de estrutura, consulta e estruturas vazias ========
    TipoEstrutura tipo_base;
    TipoEstrutura tipo_resultado;
    if(!estrutura_tipo_de_texto(config.base_type, &tipo_base)){
        return terminar_com_erro("estrutura de --base desconhecida");
    }
    if(!estrutura_tipo_de_texto(config.result_type, &tipo_resultado)){
        return terminar_com_erro("estrutura de --result desconhecida");
    }
    if(strcmp(config.reader, "nasa") != 0){
        return terminar_com_erro("leitor de --reader desconhecido");
    }

    QUERY *query = query_carregar(config.query_path);
    if(query == NULL){
        return terminar_com_erro("nao foi possivel ler o arquivo de consulta");
    }
    ESTRUTURA *base = estrutura_criar(tipo_base, true, query);       // dona dos itens
    ESTRUTURA *resultado = estrutura_criar(tipo_resultado, false, query);
    if(base == NULL || resultado == NULL){
        liberar_recursos(&resultado, &base, &query, NULL, NULL);
        return terminar_com_erro("sem memoria para criar as estruturas");
    }

    // ====== ETAPA 01: LOAD(CARGA) ========
    inicio = agora_ms();
    total_lidos = reader_nasa_carregar(config.imput_path, config.limit, base);
    load_ms = agora_ms() - inicio;

    //Validacao da leitura
    if(total_lidos < 0){
        liberar_recursos(&resultado, &base, &query, NULL, NULL);
        return terminar_com_erro("nao foi possivel ler o arquivo de entrada");
    }

    // ====== ETAPA 02: BUSCA E AVALIACAO ========
    /* Percorre a estrutura base e guarda, num vetor temporario, os itens aceitos
       e a similaridade de cada um. Assim o tempo de busca fica separado do
       tempo de insercao no resultado. */
    int capacidade_aceitos = 1024;
    ITEM **aceitos = (ITEM **)malloc(capacidade_aceitos * sizeof(ITEM *));
    double *similaridades = (double *)malloc(capacidade_aceitos * sizeof(double));
    if(aceitos == NULL || similaridades == NULL){
        liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
        return terminar_com_erro("sem memoria para guardar os resultados da busca");
    }

    inicio = agora_ms();
    estrutura_reiniciar(base);
    ITEM *item;
    while((item = estrutura_proximo(base)) != NULL){
        double similaridade = 0.0;
        if(!query_avaliar_item(query, item, &similaridade)){
            continue;
        }
        if(matched_records >= capacidade_aceitos){
            capacidade_aceitos *= 2;
            ITEM **novo_aceitos = (ITEM **)realloc(aceitos, capacidade_aceitos * sizeof(ITEM *));
            if(novo_aceitos == NULL){
                liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
                return terminar_com_erro("sem memoria para guardar os resultados da busca");
            }
            aceitos = novo_aceitos;
            double *novas_similaridades = (double *)realloc(similaridades, capacidade_aceitos * sizeof(double));
            if(novas_similaridades == NULL){
                liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
                return terminar_com_erro("sem memoria para guardar os resultados da busca");
            }
            similaridades = novas_similaridades;
        }
        aceitos[matched_records] = item;
        similaridades[matched_records] = similaridade;
        matched_records++;
    }
    search_ms = agora_ms() - inicio;

    // ====== ETAPA 03: INSERCAO NOS RESULTADOS ========
    inicio = agora_ms();
    for(int i = 0; i < matched_records; i++){
        if(!estrutura_inserir(resultado, aceitos[i], similaridades[i])){
            liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
            return terminar_com_erro("sem memoria para inserir na estrutura de resultado");
        }
    }
    result_insert_ms = agora_ms() - inicio;

    // ====== ETAPA 04: ESCRITA DO ARQUIVO DE SAIDA ========
    /* "wb" para gravar os bytes sem alteracao (o fim de linha fica igual ao da entrada).
       Grava o cabecalho e depois os itens na ordem de acesso da estrutura de
       resultado, parando em config.top_k (0 = todos). */
    inicio = agora_ms();
    FILE *saida = fopen(config.output_path, "wb");
    if(saida == NULL){
        liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
        return terminar_com_erro("nao foi possivel criar o arquivo de saida");
    }
    fprintf(saida, "%s%s", item_get_cabecalho(), item_get_terminador());
    estrutura_reiniciar(resultado);
    while((config.top_k == 0 || output_records < config.top_k) &&
          (item = estrutura_proximo(resultado)) != NULL){
        item_imprimir_csv(item, saida);
        output_records++;
    }
    fclose(saida);
    write_ms = agora_ms() - inicio;

    // ====== ETAPA 05: RELATORIO FINAL ========
    printf("STATUS=OK\n");                      //indica que a execucao ocorreu sem erros
    printf("INPUT_RECORDS=%d\n", total_lidos);  //quantidade total de registros lidos do CSV de entrada
    printf("MATCHED_RECORDS=%d\n", matched_records); //quantidade de registros que atenderam aos filtros da consulta
    printf("OUTPUT_RECORDS=%d\n", output_records);   //quantidade de registros efetivamente gravados no arquivo de saida (respeitando o --top-k)
    printf("LOAD_MS=%.3f\n", load_ms);          //Tempo da etapa de carga do dataset na memoria
    printf("SEARCH_MS=%.3f\n", search_ms);      //Tempo de execucao da consulta/busca na estrutura --base
    printf("RESULT_INSERT_MS=%.3f\n", result_insert_ms); //Tempo gasto inserindo os resultados na estrutura 
    printf("WRITE_MS=%.3f\n", write_ms);        //Tempo de gravacao do arquivo CSV de saida

    //Limpeza de memoria
    liberar_recursos(&resultado, &base, &query, aceitos, similaridades);

    return 0;
}
