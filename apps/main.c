#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "cli.h"
#include "item.h"
#include "query.h"
#include "estrutura.h"
#include "reader_antt.h"

/* Quantidade de execucoes usadas para calcular a media dos tempos. */
#define REPETICOES 5

/* Guarda as contagens e os tempos medidos em uma execucao completa. */
typedef struct {
    int total_lidos;
    int matched_records;
    int output_records;
    double load_ms;
    double search_ms;
    double result_insert_ms;
    double write_ms;
} Medicao;

/* Retorna o tempo monotônico em milissegundos, adequado para medir duracoes. */
static double agora_ms(void) {
    struct timespec tempo;
    clock_gettime(CLOCK_MONOTONIC, &tempo);
    return tempo.tv_sec * 1000.0 + tempo.tv_nsec / 1000000.0;
}

/* Confere se os argumentos obrigatorios da linha de comando foram informados. */
static bool argumentos_completos(const ConfigCLI *config) {
    return config->reader != NULL && config->imput_path != NULL &&
           config->base_type != NULL && config->result_type != NULL &&
           config->query_path != NULL && config->output_path != NULL;
}

/* Centraliza a liberacao dos recursos alocados durante uma rodada. */
static void liberar_recursos(ESTRUTURA **resultado, ESTRUTURA **base,
                             QUERY **query, ITEM **aceitos,
                             double *similaridades) {
    estrutura_apagar(resultado);
    estrutura_apagar(base);
    query_apagar(query);
    item_liberar_cabecalho();
    free(aceitos);
    free(similaridades);
}

/* Padroniza a mensagem de erro e o codigo de retorno do programa. */
static int terminar_com_erro(const char *mensagem) {
    printf("STATUS=ERROR\n");
    fprintf(stderr, "Erro: %s\n", mensagem);
    return 1;
}

/* Executa uma rodada completa, desde o carregamento ate a escrita.
   Cada rodada cria suas proprias estruturas para que os tempos sejam comparaveis. */
static int executar_uma_vez(const ConfigCLI *config,
                            TipoEstrutura tipo_base,
                            TipoEstrutura tipo_resultado,
                            Medicao *m) {
    int capacidade_aceitos = 1024;
    ITEM **aceitos = NULL;
    double *similaridades = NULL;
    QUERY *query = NULL;
    ESTRUTURA *base = NULL;
    ESTRUTURA *resultado = NULL;
    FILE *saida = NULL;
    ITEM *item;
    double inicio;

    m->total_lidos = 0;
    m->matched_records = 0;
    m->output_records = 0;
    m->load_ms = m->search_ms = m->result_insert_ms = m->write_ms = 0.0;

    query = query_carregar(config->query_path);
    if (query == NULL) {
        fprintf(stderr, "Erro: nao foi possivel ler o arquivo de consulta\n");
        liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
        return 1;
    }

    base = estrutura_criar(tipo_base, true, query);
    resultado = estrutura_criar(tipo_resultado, false, query);
    if (base == NULL || resultado == NULL) {
        fprintf(stderr, "Erro: sem memoria para criar as estruturas\n");
        liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
        return 1;
    }

    /* 1. LOAD: mede o tempo gasto para ler o arquivo e preencher a estrutura base. */
    inicio = agora_ms();
    m->total_lidos = reader_antt_carregar(config->imput_path, config->limit, base);
    m->load_ms = agora_ms() - inicio;
    if (m->total_lidos < 0) {
        fprintf(stderr, "Erro: nao foi possivel ler o arquivo de entrada\n");
        liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
        return 1;
    }

    aceitos = malloc((size_t)capacidade_aceitos * sizeof(*aceitos));
    similaridades = malloc((size_t)capacidade_aceitos * sizeof(*similaridades));
    if (aceitos == NULL || similaridades == NULL) {
        fprintf(stderr, "Erro: sem memoria para guardar os resultados da busca\n");
        liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
        return 1;
    }

    /* 2. SEARCH: percorre a base e avalia os itens; os aceitos ficam em vetores
       temporarios para que a insercao na estrutura de resultado seja medida a parte. */
    inicio = agora_ms();
    estrutura_reiniciar(base);
    while ((item = estrutura_proximo(base)) != NULL) {
        double similaridade = 0.0;
        if (!query_avaliar_item(query, item, &similaridade)) {
            continue;
        }

        if (m->matched_records >= capacidade_aceitos) {
            int nova_capacidade = capacidade_aceitos * 2;
            ITEM **novo_aceitos = realloc(aceitos,
                (size_t)nova_capacidade * sizeof(*aceitos));
            if (novo_aceitos == NULL) {
                fprintf(stderr, "Erro: sem memoria para expandir os resultados\n");
                liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
                return 1;
            }
            aceitos = novo_aceitos;

            double *novas_similaridades = realloc(similaridades,
                (size_t)nova_capacidade * sizeof(*similaridades));
            if (novas_similaridades == NULL) {
                fprintf(stderr, "Erro: sem memoria para expandir as similaridades\n");
                liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
                return 1;
            }
            similaridades = novas_similaridades;
            capacidade_aceitos = nova_capacidade;
        }

        aceitos[m->matched_records] = item;
        similaridades[m->matched_records] = similaridade;
        m->matched_records++;
    }
    m->search_ms = agora_ms() - inicio;

    /* 3. RESULT INSERT: insere na estrutura de resultado os itens aceitos na busca. */
    inicio = agora_ms();
    for (int i = 0; i < m->matched_records; i++) {
        if (!estrutura_inserir(resultado, aceitos[i], similaridades[i])) {
            fprintf(stderr, "Erro: sem memoria para inserir no resultado\n");
            liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
            return 1;
        }
    }
    m->result_insert_ms = agora_ms() - inicio;

    /* 4. WRITE: grava o cabecalho e os itens encontrados, respeitando --top-k. */
    inicio = agora_ms();
    saida = fopen(config->output_path, "wb");
    if (saida == NULL) {
        fprintf(stderr, "Erro: nao foi possivel criar o arquivo de saida\n");
        liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
        return 1;
    }

    fprintf(saida, "%s%s", item_get_cabecalho(), item_get_terminador());
    estrutura_reiniciar(resultado);
    while ((config->top_k == 0 || m->output_records < config->top_k) &&
           (item = estrutura_proximo(resultado)) != NULL) {
        item_imprimir_csv(item, saida);
        m->output_records++;
    }
    if (fclose(saida) != 0) {
        saida = NULL;
        fprintf(stderr, "Erro: falha ao fechar o arquivo de saida\n");
        liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
        return 1;
    }
    saida = NULL;
    m->write_ms = agora_ms() - inicio;

    liberar_recursos(&resultado, &base, &query, aceitos, similaridades);
    return 0;

}

/*
 * Calcula a media aritmetica dos tempos das cinco execucoes.
 */
static double media(double valores[REPETICOES]) {
    double soma = 0.0;

    for (int i = 0; i < REPETICOES; i++) {
        soma += valores[i];
    }

    return soma / REPETICOES;
}

int main(int argc, char *argv[]) {
    ConfigCLI config = cli_parse(argc, argv);
    if (!argumentos_completos(&config)) {
        return terminar_com_erro("faltou argumento obrigatorio (--reader, --input, --base, --result, --query, --output)");
    }
    if (config.top_k < 0) config.top_k = 0;
    if (config.limit < 0) config.limit = 0;

    TipoEstrutura tipo_base, tipo_resultado;
    if (!estrutura_tipo_de_texto(config.base_type, &tipo_base)) {
        return terminar_com_erro("estrutura de --base desconhecida");
    }
    if (!estrutura_tipo_de_texto(config.result_type, &tipo_resultado)) {
        return terminar_com_erro("estrutura de --result desconhecida");
    }
    if (strcmp(config.reader, "antt") != 0 && strcmp(config.reader, "nasa") != 0) {
        return terminar_com_erro("leitor de --reader desconhecido");
    }

    /* Cada vetor guarda os tempos de uma etapa ao longo das repeticoes. */
    Medicao medicoes[REPETICOES];
    double loads[REPETICOES], buscas[REPETICOES];
    double insercoes[REPETICOES], escritas[REPETICOES];

    /* Repete o processamento completo e separa os tempos de cada etapa. */
    for (int i = 0; i < REPETICOES; i++) {
        if (executar_uma_vez(&config, tipo_base, tipo_resultado, &medicoes[i]) != 0) {
            return terminar_com_erro("a execucao foi interrompida antes de completar as 5 repeticoes");
        }
        loads[i] = medicoes[i].load_ms;
        buscas[i] = medicoes[i].search_ms;
        insercoes[i] = medicoes[i].result_insert_ms;
        escritas[i] = medicoes[i].write_ms;
    }

    /* As contagens correspondem a ultima rodada; os tempos
   exibidos sao as medias aritmeticas das cinco execucoes. */
    printf("STATUS=OK\n");
    printf("INPUT_RECORDS=%d\n", medicoes[REPETICOES - 1].total_lidos);
    printf("MATCHED_RECORDS=%d\n", medicoes[REPETICOES - 1].matched_records);
    printf("OUTPUT_RECORDS=%d\n", medicoes[REPETICOES - 1].output_records);
    printf("LOAD_MS=%.3f\n", media(loads));
    printf("SEARCH_MS=%.3f\n", media(buscas));
    printf("RESULT_INSERT_MS=%.3f\n", media(insercoes));
    printf("WRITE_MS=%.3f\n", media(escritas));
    return 0;
}