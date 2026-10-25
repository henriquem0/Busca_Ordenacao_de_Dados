#ifndef READER_ANTT_H
#define READER_ANTT_H

#include "estrutura.h"

/* - le o csv da antt e vai inserindo cada registro direto na estrutura --base 
- caminho_csv: o caminho do arquivo no disco, o const garante que nao sera alterado
- limit: o limite de registros a serem lidos definido pelo (--limit), 0 = le tudo 
- base: estrutura que recebe os itens durante a leitura antes de ler os registros, define o cabecalho (nomes das colunas) no item. retorna a quantidade de registros lidos ou -1 se houver erro (arquivo inexistente, falta de memoria...). quem chamou deve chamar item_liberar_cabecalho() no final do programa.*/
int reader_antt_carregar(const char *caminho_csv, int limit, ESTRUTURA *base);

#endif
