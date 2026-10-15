#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#include "cli.h"
#include "item.h"
#include "reader_nasa.h"

int main(int argc, char *argv[]){
    //Processa as flags da linha de comando
    ConfigCLI config = cli_parse(argc, argv);

    //Variaveis de contagem     
    int total_lidos = 0;
    int matched_records = 0;
    int output_records = 0;

    //Variaveis de medicao de tempo (milissegundos)
    struct timespec t_inicio, t_fim;
    double load_ms = 0.0;
    double search_ms = 0.0;
    double result_insert = 0.0;
    double write_ms = 0.0;
    
    // ====== ETAPA 01: LOAD(CARGA) ========
    double inicio = clock();
    ITEM **base_dados = reader_nasa_csv(config.imput_path, config.limit, &total_lidos);
    double fim = clock();
    //Finaliza medicao
    load_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0; 

    //Validacao da leitura
    if(base_dados == NULL && total_lidos == 0 && config.limit != 0){
        printf("STATUS=ERROR\n");
        return 1;
    }

    // ====== ETAPA 02: BUSCA E AVALIACAO ======== [implementar]
    /*inicio = clock();
    percorre base_dados e avalia
    fim = clock();
    search_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
    */

    // ====== ETAPA 03: INSERCAO NOS RESULTADOS ======== [implementar]
    /*inicio = clock();
    insere os itens aceitos na estrutura
    fim = clock();
    result_insert_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
    */

    // ====== ETAPA 04: ESCRITA DO ARQUIVO DE SAIDA ======== [implementar]
    /*inicio = clock();
    grava em config.output respeitando o config.top-k
    fim = clock();
    write_ms = ((double)(fim - inicio) / CLOCKS_PER_SEC) * 1000.0;
    */

    // ====== ETAPA 05: RELATORIO FINAL ========
    printf("STATUS=OK\n");                      //indica que a execucao ocorreu sem erros
    printf("INPUT_RECORDS=%d\n", total_lidos);  //quantidade total de registros lidos do CSV de entrada
    printf("MATCHED_RECORDS=%d\n", 0);           //quantidade de registros que atenderam aos filtros da consulta
    printf("OUTPUT_RECORDS=%d\n", 0);           //quantidade de registros efetivamente gravados no arquivo de saida (respeitando o --top-k)
    printf("LOAD_MS=%.3f\n", load_ms);          //Tempo da etapa de carga do dataset na memoria
    printf("SEARCH_MS=%.3f\n", 0.0);            //Tempo de execucao da consulta/busca na estrutura --base
    printf("RESULT_INSERT_MS=%.3f\n", 0.0);     //Tempo gasto inserindo os resultados na estrutura 
    printf("WRITE_MS=%.3f\n", 0.0);             //Tempo de gravacao do arquivo CSV de saida

    //Limpeza de memoria
    reader_nasa_desalocar(&base_dados, total_lidos);

    return 0;
}