#ifndef CLI_H
#define CLI_H

typedef struct{
    char *reader;           // guarda o leitor
    char *imput_path;       // guarda o caminho do arquivo
    char *base_type;        // guarda a estrutura pincipal
    char *result_type;      // guarda a estrutura de resultado
    char *query_path;       // guarda o arquivo de consulta
    char *output_path;      // guarda o arquivo de saida
    int top_k;              // guarda a quantidade top_k
    int limit;              // guarda o limite de registros
} ConfigCLI;


//leitura do terminal e prenchimento da struct.
ConfigCLI cli_parse(int argc, char *argv[]);

#endif

