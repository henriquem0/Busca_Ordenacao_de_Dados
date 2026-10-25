#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "item.h"

struct item_{
    char *linha;    // linha original do csv, sem o fim de linha (usada na saida)
    char **campos;  // valor de cada coluna, sem as aspas
    int qtd_campos;
    char id[16]; //O id é um char para evitar mexer em todos os TADs de listas
};

// cabecalho do dataset. fica no modulo porque e igual para todos os itens de um mesmo arquivo: o leitor define uma vez e os itens consultam.
static char **nomes_colunas = NULL;
static int qtd_colunas = 0;
static char *cabecalho_original = NULL;
static char terminador_linha[3] = "\n";
static int prox_id = 1;


// funcao auxiliar para duplicar as strings e alocar dinamicamente
static char *meu_strdup(const char *str){
    if(str == NULL) return NULL;


    int tam = strlen(str) + 1; // calcula o tamanho do texto com o terminador
    char *copia = (char *)malloc(tam); // reserva memoria para a copia


    if(copia != NULL){
        strcpy(copia, str); // copia o texto para o espaco reservado
    }

    return copia;
}




// libera um vetor de textos com 'qtd' posicoes
static void liberar_campos(char **campos, int qtd){
    if(campos == NULL){
        return;
    }


    for(int i = 0; i < qtd; i++){
        free(campos[i]); // libera cada coluna antes do vetor
    }


    free(campos); // libera o vetor de ponteiros
}




// compara dois textos ignorando maiusculas e minusculas ("true" == "true")
static bool textos_iguais_sem_caixa(const char *a, const char *b)
{
    while(*a != '\0' && *b != '\0')
    {
        if(tolower((unsigned char) *a) != tolower((unsigned char) *b))
        {
            return false;
        }
        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}



static char detectar_delimitador(const char *linha) {
    int virgulas = 0;
    int ponto_virgulas = 0;
    int entre_aspas = 0;

    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == '"') {
            // Aspas duplicadas dentro de um campo escapado
            if (entre_aspas && linha[i + 1] == '"') {
                i++;
                continue;
            }
            entre_aspas = !entre_aspas;
        } else if (!entre_aspas) {
            if (linha[i] == ',') {
                virgulas++;
            } else if (linha[i] == ';') {
                ponto_virgulas++;
            }
        }
    }

    return (virgulas > ponto_virgulas) ? ',' : ';';
}


// divide uma linha csv em colunas. uma virgula dentro de aspas faz parte do texto, por isso o laco controla se estamos "entre aspas". colunas vazias (",,") viram texto vazio. devolve a quantidade de colunas ou -1 se faltar memoria.
static int dividir_linha_csv(const char *linha, char ***campos_saida) {
    char delimitador = detectar_delimitador(linha);
    int qtd = 1;
    bool entre_aspas = false;

    // Conta as colunas, ignorando separadores dentro de aspas
    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == '"') {
            if (entre_aspas && linha[i + 1] == '"') {
                i++;
                continue;
            }
            entre_aspas = !entre_aspas;
        } else if (linha[i] == delimitador && !entre_aspas) {
            qtd++;
        }
    }

    char *buffer = malloc(strlen(linha) + 1);
    char **campos = calloc(qtd, sizeof(char *));

    if (buffer == NULL || campos == NULL) {
        free(buffer);
        free(campos);
        return -1;
    }

    int indice_campo = 0;
    int tam_buffer = 0;
    entre_aspas = false;

    // Separa os campos e trata aspas duplicadas
    for (int i = 0; ; i++) {
        char atual = linha[i];

        if (atual == '"') {
            if (entre_aspas && linha[i + 1] == '"') {
                buffer[tam_buffer++] = '"';
                i++;
            } else {
                entre_aspas = !entre_aspas;
            }
        } else if ((atual == delimitador && !entre_aspas)
                   || atual == '\0') {
            buffer[tam_buffer] = '\0';
            campos[indice_campo] = meu_strdup(buffer);

            if (campos[indice_campo] == NULL) {
                liberar_campos(campos, indice_campo);
                free(buffer);
                return -1;
            }

            indice_campo++;
            tam_buffer = 0;

            if (atual == '\0') {
                break;
            }
        } else {
            buffer[tam_buffer++] = atual;
        }
    }
    free(buffer);
    *campos_saida = campos;
    return qtd;
}


// guarda uma copia da linha sem o '\r' e o '\n' do final
static char *copiar_sem_fim_de_linha(const char *linha){
    char *copia = meu_strdup(linha);
    if(copia != NULL){
        copia[strcspn(copia, "\r\n")] = '\0'; // remove a quebra de linha encontrada
    }
    return copia;
}

// 1) cabecalho
void item_liberar_cabecalho(void){
    liberar_campos(nomes_colunas, qtd_colunas); // libera os nomes das colunas antigas
    nomes_colunas = NULL;
    qtd_colunas = 0;
    free(cabecalho_original);
    cabecalho_original = NULL;
    strcpy(terminador_linha, "\n"); // volta ao terminador padrao
    prox_id = 1;
}

void item_definir_cabecalho(const char *linha_cabecalho){
    item_liberar_cabecalho();
    if(linha_cabecalho == NULL){
        return;
    }
    // descobre se o arquivo termina as linhas com "\r\n" ou so "\n"
    if(strstr(linha_cabecalho, "\r\n") != NULL){
        strcpy(terminador_linha, "\r\n"); // preserva o terminador usado pelo arquivo
    }
    cabecalho_original = copiar_sem_fim_de_linha(linha_cabecalho);
    if(cabecalho_original == NULL){
        return;
    }
    // alguns arquivos comecam com 3 bytes invisiveis (bom): nao fazem parte do nome da coluna
    const char *inicio_nomes = cabecalho_original;
    if((unsigned char)inicio_nomes[0] == 0xEF && (unsigned char)inicio_nomes[1] == 0xBB &&
       (unsigned char)inicio_nomes[2] == 0xBF){
        inicio_nomes += 3;
    }
    qtd_colunas = dividir_linha_csv(inicio_nomes, &nomes_colunas); // separa o cabecalho em nomes
    if(qtd_colunas < 0){
        qtd_colunas = 0;
        nomes_colunas = NULL;
    }
}

const char *item_get_cabecalho(void){
    return cabecalho_original;
}

const char *item_get_terminador(void){
    return terminador_linha;
}

// 2) construtor e destruidor
ITEM *item_criar(const char *linha_csv){
    if(linha_csv == NULL){
        return NULL;
    }
    ITEM *item = (ITEM *)malloc(sizeof(ITEM)); // reserva memoria para o novo registro
    if(item == NULL){
        return NULL;
    }
    item->linha = copiar_sem_fim_de_linha(linha_csv); // guarda a linha sem a quebra final
    item->campos = NULL;
    item->qtd_campos = 0;
    item->id[0] = '\0';

    // linha vazia nao e um registro
    if(item->linha == NULL || item->linha[0] == '\0'){
        free(item->linha);
        free(item);
        return NULL;
    }
    item->qtd_campos = dividir_linha_csv(item->linha, &item->campos); // separa a linha nas colunas
    if(item->qtd_campos < 0){
        free(item->linha);
        free(item);
        return NULL;
    }

    snprintf(item->id, sizeof(item->id), "%d", prox_id++);

    return item;
}

void item_apagar(ITEM **item){
    if(item == NULL || *item == NULL){
        return;
    }
    liberar_campos((*item)->campos, (*item)->qtd_campos);
    free((*item)->linha); // libera a linha original
    free(*item); // libera a estrutura do item
    *item = NULL;
}

// 3) acesso pelo nome da coluna
const char *item_get_campo(const ITEM *item, const char *nome_campo){
    if(item == NULL || nome_campo == NULL){
        return NULL;
    }
    for(int i = 0; i < qtd_colunas && i < item->qtd_campos; i++){
        if(strcmp(nomes_colunas[i], nome_campo) == 0){ // procura a coluna pelo nome
            if(item->campos[i][0] == '\0'){
                return NULL; // valor ausente
            }
            return item->campos[i];
        }
    }
    return NULL;
}

bool item_converter_numero(const char *texto, double *valor){
    if(texto == NULL || valor == NULL){
        return false;
    }
    if(textos_iguais_sem_caixa(texto, "true")){
        *valor = 1.0;
        return true;
    }
    if(textos_iguais_sem_caixa(texto, "false")){
        *valor = 0.0;
        return true;
    }

    char *copia = meu_strdup(texto);

    if(!copia)
    {return false;}

    char *p = strchr(copia, ',');

    if(p)
    {*p = '.';}
    
    char *fim;
    
    double n = strtod(copia, &fim);
    
    bool ok = (fim != copia);

    while(ok && *fim == ' ')
    {fim++;}

    if(ok && *fim != '\0')
    {ok = false;}

    free(copia);

    if(!ok)
    {return false;}

    *valor = n; return true;
}

bool item_get_campo_numerico(const ITEM *item, const char *nome_campo, double *valor){
    return item_converter_numero(item_get_campo(item, nome_campo), valor);
}

// 4) getters
const char *item_get_consessionaria(const ITEM *item){
    return item_get_campo(item, "concessionaria");
}

int item_get_id_sigicor(const ITEM *item)
{
    double valor = 0.0;
    if(!item_get_campo_numerico(item, "id_sigicor", &valor))
    {
        return (int) -1;
    }
    return (int) valor;
}

const char *item_get_emissao_sei(const ITEM *item)
{
    return item_get_campo(item, "emitida_no_SEI");
}

const char *item_get_autorizacao_sei(const ITEM *item)
{
    return item_get_campo(item, "processo_SEI_Autorização");
}

int item_get_numero_sei(const ITEM *item)
{
    double valor = 0.0;
    if(!item_get_campo_numerico(item, "n_SEI", &valor))
    {
        return (int) -1;
    }
    return (int)valor;
}

const char *item_get_item_do_per(const ITEM *item)
{
    return item_get_campo(item, "item_do_PER");
}

const char *item_get_origem_per(const ITEM *item)
{
    return item_get_campo(item, "origem_PER");
}

const char *item_get_tipo_obra(const ITEM *item)
{
    return item_get_campo(item, "tipo_de_Obra");
}

const char *item_get_rodovia(const ITEM *item)
{
    return item_get_campo(item, "rodovia");
}

double item_get_km_inicio(const ITEM *item)
{
    double valor = 0.0;
    if(!item_get_campo_numerico(item, "km_Inicial", &valor))
    {
        return (double) -1.0;
    }
    return (double) valor;
}

double item_get_km_final(const ITEM *item)
{
    double valor = 0.0;
    if(!item_get_campo_numerico(item, "km_Final", &valor))
    {
        return (double) -1.0;
    }
    return (double) valor;
}

const char *item_get_prazo(const ITEM *item)
{
    return item_get_campo(item, "prazo_Concessão");
}

const char *item_get_tipo_projeto(const ITEM *item)
{
    return item_get_campo(item, "tipo_de_Projeto");
}

const char *item_get_status_projeto(const ITEM *item)
{
    return item_get_campo(item, "situação_do_Projeto");
}

const char *item_get_processo(const ITEM *item)
{
    return item_get_campo(item, "processo");
}

double item_get_KmInicio(const ITEM *item)
{
    double valor = 0.0;

    if(!item_get_campo_numerico(item, "km_inicial", &valor))
    {
        return (double) -1.0;
    }
    return (double) valor;
}

double item_get_KmFim(const ITEM *item)
{
    double valor = 0.0;
    if(!item_get_campo_numerico(item, "km_final", &valor))
    {
        return (double) -1.0;
    }
    return (double) valor;
}
// O id é o número da linha
const char *item_get_identificador(const ITEM *item){
    if(item == NULL || item->id[0] == '\0'){
        return NULL;
    }
    return item->id;
}

// 5) impressao: a linha sai exatamente como estava no arquivo de entrada
void item_imprimir_csv(const ITEM *item, FILE *fp){
    if(item == NULL || fp == NULL){
        return;
    }
    fprintf(fp, "%s%s", item->linha, terminador_linha);
}
