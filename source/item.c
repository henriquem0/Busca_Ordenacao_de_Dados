#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> 
#include "item.h"

struct item_{
    char *asteroid_designation; 
    char *asteroid_fullname;
    char *close_approach_date;
    int year;
    int month;
    double distance_au;
    double velocity_km_s;
    float absolute_magnitude; 
    int days_until_approach;
    bool is_past_event;
    bool is_future_event;
    double risk_score;
    int panic_level;
    char *threat_category;
    char *panic_verdict;
    bool on_sentry_list;
    double sentry_impact_prob;
    int sentry_torino_scale;
    float sentry_palermo_scale;
    int sentry_diameter_km;

};

//Funcao auxiliar para duplicar as strings e alocar dinamicamente
static char *meu_strdup(const char *str){
    if(str == NULL) return NULL;
    int tam = strlen(str) + 1;
    char *copia = (char *)malloc(tam);
    if(copia != NULL){
        strcpy(copia, str);
    }
    return copia;
}

// 1) Construtor e Destruidor
ITEM *item_criar(const char *linha_csv){

    // - Trava de seguranca para garantir que foi criado
    if(linha_csv == NULL){
        return NULL;
    }

    ITEM *item = (ITEM *)malloc(sizeof(ITEM));
    if(item == NULL){
        return NULL;
    }

    // - "strup" calcula o tamanho da string original, o equivalente a um strlen
    // - e chama a funcao malloc() para reservar esse espaco na heap
    //  - copiamos a linha_csv para o buffer para utiliza-la de forma temporaria
    // - como a linha_csv e const precisamos colocar em outra para conseguir manipular
    char *buffer = meu_strdup(linha_csv);
    
    /* - Operacoes utilizadas
        -- atoi (ASCII to INT): converte uma string que contem um numero inteiro para o tipo int
        -- atof (ASCII to Float/Double): converte uma string que contem um numero float ou double para um tipo double
        -- strcmp (string compare): compara duas strings e retorna 0 se forem identica
                    < 0 se a primeira string vier antes que a segunda
                    > 0  se a primeira string vier depois da segunda
        -- strtok (string tokenize): corta uma string em pedacos (tokens) utilizando um ou mais
                    caracteres delimitadores (nesse caso, ",", "\r" ,"\n") e substitui por \0 nesses campos
                    cortando a string
        -- strdup (string duplicate): cria uma copia identica de uma string na memoria dinamica
    */
    //------- Campo 1: Texto (designation) -------
    char *token = strtok(buffer, ",\r\n"); 
    if(token != NULL){
        item->asteroid_designation = meu_strdup(token);
    }

    //------- Campo 2: Texto (fullname) ----------
    /*o strtok memoriza (guarda na "Data Segment") a ultima string passada por ele ao utilizarmos o null ele apenas continua 
      de onde ele parou de fatiar a string, fazendo com que pudessemos cortar ela aos poucos. */
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->asteroid_fullname = meu_strdup(token);
    }

    //------- Campo 3: Texto (date) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->close_approach_date = meu_strdup(token);
    }

    //------- Campo 4: inteiro (year) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->year = atoi(token);
    }

    //------- Campo 5: inteiro (month) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->month = atoi(token);
    }

    //------- Campo 6: double (distance_au) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->distance_au = atof(token);
    }

    //------- Campo 7: double (velocity) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->velocity_km_s = atof(token);
    }

    //------- Campo 8: float (magnitude) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->absolute_magnitude = atof(token);
    }

    //------- Campo 9: inteiro (days) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->days_until_approach = atoi(token);
    }

    //------- Campo 10: booleano (past_event) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->is_past_event = (strcmp(token, "TRUE") == 0 || strcmp(token, "1") == 0);
    }

    //------- Campo 11: booleano (future_event) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->is_future_event = (strcmp(token, "TRUE") == 0 || strcmp(token, "1") == 0);
    }

    //------- Campo 12: double (risk) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->risk_score = atof(token);
    }

    //------- Campo 13: inteiro (panic_level) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->panic_level = atoi(token);
    }

    //------- Campo 14: Texto (threat_category) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->threat_category = meu_strdup(token);
    }
    //------- Campo 15: Texto (panic_verdict) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->panic_verdict = meu_strdup(token);
    }
    //------- Campo 16: booleano (on_sentry_list) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->on_sentry_list = (strcmp(token, "TRUE") == 0 || strcmp(token, "1") == 0);
    }

    //------- Campo 17: double (impact_prob) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->sentry_impact_prob = atof(token);
    }

    //------- Campo 18: inteiro (torino_scale) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->sentry_torino_scale = atoi(token);
    }

    //------- Campo 19: float (palermo_scale) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->sentry_palermo_scale = atof(token);
    }

    //------- Campo 20: inteiro (diameter_km) -------
    token = strtok(NULL, ",\r\n"); 
    if(token != NULL){
        item->sentry_diameter_km = atoi(token);
    }

    free(buffer); // Libera a folha de rascunho temporaria
    return item; // Retorna o ponteiro para o ITEM totalmente preenchido
}


void item_apagar(ITEM **item){
    if(item == NULL || *item == NULL){
        return;
    }

    /* Ao liberar a memoria dessa forma garante que nao tera 
       nenhum ponteiro solto, evitando o dangling pointer.*/
    free((*item)->asteroid_designation);
    (*item)->asteroid_designation = NULL;

    free((*item)->asteroid_fullname);
    (*item)->asteroid_fullname = NULL;

    free((*item)->close_approach_date);
    (*item)->close_approach_date = NULL;

    free((*item)->threat_category);
    (*item)->threat_category = NULL;

    free((*item)->panic_verdict);
    (*item)->panic_verdict = NULL;

    free(*item);

    *item = NULL;
}

// 2) GETTERS para as buscas

// - Filtros (FILTER)
double item_get_distancia(const ITEM *item){
    if(item == NULL){
        return 0.0;
    }
    return item->distance_au;
}
bool item_get_is_future_event(const ITEM *item){
    if(item == NULL){
        return false;
    }
    return item->is_future_event;
}

// - Similaridades (SIM)
double item_get_velocidade(const ITEM *item){
    if(item == NULL){
        return 0.0;
    }
    return item->velocity_km_s; 
}

double item_get_risk_score(const ITEM *item){
    if(item == NULL){
        return 0.0;
    }
    return item->risk_score; 
}

// - Por Agrupamento e Listas Avancadas (GROUP e CROSS)
const char *item_get_threat_category(const ITEM *item){
    if(item == NULL){
        return NULL;
    }
    return item->threat_category;
}

int item_get_year(const ITEM *item){
    if(item == NULL){
        return 0;
    }
    return item->year;
}

// - Para identificacao
const char *item_get_designation(const ITEM *item){ 
    if(item == NULL){
        return NULL;
    }
    return item->asteroid_designation;
}

// 3) Impressao
void item_imprimir_csv(const ITEM *item, FILE *fp){
    if(item == NULL){
        return;
    }

    fp = fopen("dados/asteroids.csv", "w");
    if(fp == NULL){
        return; //Erro ao abrir o arquivo.
    }
    
    fprintf(fp, "%s,%s,%s,%d,%d,%.6f,%.6f,%.2f,%d,%s,%s,%.6f,%d,%s,%s,%s,%.8f,%d,%.2f,%d",
        item->asteroid_designation,
        item->asteroid_fullname,
        item->close_approach_date,
        item->year,
        item->month,
        item->distance_au,
        item->velocity_km_s,
        item->absolute_magnitude,
        item->days_until_approach,
        item->is_past_event ? "TRUE" : "FALSE",
        item->is_future_event ? "TRUE" : "FALSE",
        item->risk_score,
        item->panic_level,
        item->threat_category,
        item->panic_verdict,
        item->on_sentry_list ? "TRUE" : "FALSE",
        item->sentry_impact_prob,
        item->sentry_torino_scale,
        item->sentry_palermo_scale,
        item->sentry_diameter_km);

    fclose(fp);
}