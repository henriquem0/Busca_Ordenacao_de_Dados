#ifndef CLI_H
#define CLI_H

typedef struct{
    char *reader;           // guarda o leitor (ex: "nasa")
    char *imput_path;       // guarda o caminho do arquivo (ex: "dados/asteroids.csv")
    char *base_type;        // guarda a estrutura pincipal (ex: "lista_enc")
    char *result_type;      // guarda a estrutura de resultado (ex: "lista_cruz")
    char *query_path;       // guarda o arquivo de consulta (ex: consultas/q1.txt)
    char *output_path;      // guarda o arquivo de saida (ex: "resultado.csv")
    int top_k;              // guarda a quantidade top_k (ex: "100")
    int limit;              // guarda o limite de registros (ex: "1000")
} ConfigCLI;


//leitura do terminal e prenchimento da struct.
ConfigCLI cli_parse(int argc, char *argv[]);

#endif

