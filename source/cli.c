#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "cli.h"

ConfigCLI cli_parse(int argc, char *argv[]){
    ConfigCLI config;

    // 1) inicialmente define-se como null e indefinido
    // cada um para que nao haja lixo de memoria dentro.
    config.reader = NULL; 
    config.imput_path = NULL;
    config.base_type = NULL; 
    config.result_type = NULL;
    config.query_path = NULL;
    config.output_path = NULL;
    config.top_k = -1;
    config.limit = -1;

    // 2) Olhar cada palavra digitada no terminal, por meio de um laco
    // - comeca pelo argv[1] pois o argv[0] e o nome do proprio programa
    // - (./programa)
    
    for(int i = 1; i < argc; i++){
        /* - (i + 1) usamos como seguranca para evitar o segmentation fault*/
        if(strcmp(argv[i], "--reader") == 0 && (i + 1) < argc){
            /* - o config reader devera ser igual a argv[i + 1]
               - pois nesse caso a posicao "i" ira conter a palavra "--reader"
               - na proxima que ira conter o comando que sera armazenado. */
            config.reader = argv[i + 1]; 
        }

        else if(strcmp(argv[i], "--input") == 0 && (i + 1) < argc){
            config.imput_path = argv[i + 1];
        }

        else if(strcmp(argv[i], "--base") == 0 && (i + 1) < argc){
            config.base_type = argv[i + 1];
        }

        else if(strcmp(argv[i], "--result") == 0 && (i + 1) < argc){
            config.result_type = argv[i + 1];
        }
        
        else if(strcmp(argv[i], "--query") == 0 && (i + 1) < argc){
            config.query_path = argv[i + 1];
        }

        else if(strcmp(argv[i], "--output") == 0 && (i + 1) < argc){
            config.output_path = argv[i + 1];
        } 

        else if(strcmp(argv[i], "--top-k") == 0 && (i + 1) < argc){
            /* - A funcao "atoi" e usada para converter o texto para numero*/
            config.top_k = atoi(argv[i + 1]);
        }
        
        else if(strcmp(argv[i], "--limit") == 0 && (i + 1) < argc){
            config.limit = atoi(argv[i + 1]);
        }
    }

    return config;
}