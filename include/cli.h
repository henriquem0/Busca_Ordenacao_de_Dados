#ifndef CLI_H
#define CLI_H

typedef struct{
    char *reader;           // Guarda o leitor (ex: "nasa")
    char *imput_path;       // Guarda o caminho do arquivo (ex: "dados/asteroids.csv")
    char *base_type;        // Guarda a estrutura pincipal (ex: "lista_enc")
    char *result_type;      // Guarda a estrutura de resultado (ex: "lista_cruz")
    char *query_path;       // Guarda o arquivo de consulta (ex: consultas/q1.txt)
    char *output_path;      // Guarda o arquivo de saida (ex: "resultado.csv")
    int top_k;              // Guarda a quantidade top_k (ex: "100")
    int limit;              // Guarda o limite de registros (ex: "1000")
} ConfigCLI;


/* - Leitura do terminal e prenchimento da struct. */
ConfigCLI cli_parse(int argc, char *argv[]);

#endif

