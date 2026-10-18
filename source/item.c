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
};

// cabecalho do dataset. fica no modulo porque e igual para todos os itens de um mesmo arquivo: o leitor define uma vez e os itens consultam.
static char **nomes_colunas = NULL;
static int qtd_colunas = 0;
static char *cabecalho_original = NULL;
static char terminador_linha[3] = "\n";




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
static bool textos_iguais_sem_caixa(const char *a, const char *b){

    while(*a != '\0' && *b != '\0'){
        if(tolower((unsigned char)*a) != tolower((unsigned char)*b)){
            return false;
        }
        a++;
        b++;
    }



    return *a == '\0' && *b == '\0';
}





// divide uma linha csv em colunas. uma virgula dentro de aspas faz parte do texto, por isso o laco controla se estamos "entre aspas". colunas vazias (",,") viram texto vazio. devolve a quantidade de colunas ou -1 se faltar memoria.
static int dividir_linha_csv(const char *linha, char ***campos_saida){
    int qtd = 1;
    bool entre_aspas = false;

    for(int i = 0; linha[i] != '\0'; i++){
        if(linha[i] == '"'){
            entre_aspas = !entre_aspas;
        }
        else if(linha[i] == ',' && !entre_aspas){
            qtd++;
        }
    }

    char *buffer = (char *)malloc(strlen(linha) + 1); // cria um buffer para montar cada campo

    char **campos = (char **)calloc(qtd, sizeof(char *)); // cria o vetor de colunas zerado


    if(buffer == NULL || campos == NULL){
        free(buffer);
        free(campos);
        return -1;
    }

    int indice_campo = 0;
    int tam_buffer = 0;
    entre_aspas = false;


    for(int i = 0; ; i++){
        char atual = linha[i];


        if(atual == '"'){
            if(entre_aspas && linha[i + 1] == '"'){
                buffer[tam_buffer++] = '"'; // transforma duas aspas csv em uma
                i++;
            }
            else{
                entre_aspas = !entre_aspas;
            }
        }
        else if((atual == ',' && !entre_aspas) || atual == '\0'){
            buffer[tam_buffer] = '\0';
            campos[indice_campo] = meu_strdup(buffer); // salva uma copia do campo montado
            if(campos[indice_campo] == NULL){
                liberar_campos(campos, indice_campo);
                free(buffer); // libera o buffer temporario
                return -1;
            }
            indice_campo++;
            tam_buffer = 0;
            if(atual == '\0'){
                break;
            }
        }
        else{
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
    char *fim;
    double numero = strtod(texto, &fim); // converte o texto e aponta onde a conversao parou
    if(fim == texto){
        return false; // nao comecou com numero
    }
    while(*fim == ' '){
        fim++;
    }
    if(*fim != '\0'){
        return false; // sobrou texto depois do numero
    }
    *valor = numero;
    return true;
}

bool item_get_campo_numerico(const ITEM *item, const char *nome_campo, double *valor){
    return item_converter_numero(item_get_campo(item, nome_campo), valor);
}

// 4) getters
double item_get_distancia(const ITEM *item){
    double valor = 0.0;
    item_get_campo_numerico(item, "distance_au", &valor);
    return valor;
}

bool item_get_is_future_event(const ITEM *item){
    const char *texto = item_get_campo(item, "is_future_event");
    return texto != NULL && textos_iguais_sem_caixa(texto, "true");
}

double item_get_velocidade(const ITEM *item){
    double valor = 0.0;
    item_get_campo_numerico(item, "velocity_km_s", &valor);
    return valor;
}

double item_get_risk_score(const ITEM *item){
    double valor = 0.0;
    item_get_campo_numerico(item, "risk_score", &valor);
    return valor;
}

const char *item_get_threat_category(const ITEM *item){
    return item_get_campo(item, "threat_category");
}

int item_get_year(const ITEM *item){
    double valor = 0.0;
    item_get_campo_numerico(item, "year", &valor);
    return (int)valor;
}

// primeira coluna da linha (vazia = ausente)
const char *item_get_identificador(const ITEM *item){
    if(item == NULL || item->qtd_campos < 1 || item->campos[0][0] == '\0'){
        return NULL;
    }
    return item->campos[0];
}

const char *item_get_designation(const ITEM *item){
    return item_get_campo(item, "asteroid_designation");
}
// 5) impressao: a linha sai exatamente como estava no arquivo de entrada
void item_imprimir_csv(const ITEM *item, FILE *fp){
    if(item == NULL || fp == NULL){
        return;
    }
    fprintf(fp, "%s%s", item->linha, terminador_linha);
}
