#ifndef READER_NASA_H
#define READER_NASA_H

#include "item.h"

/* - Recebe o caminho do CSV, o limite de registros e um ponteiro para retornar
   o total lido 
   - Retorn um vetor dinamico de ponteiros para ITEM (ITEM **)
   
   - caminho_csv: o caminho do arquivo no disco, o const garante que nao sera alterado
   - limit: o limite de registros a serem lidos definido pelo (--limit)
   - total_lidos: o total de asteroides que foram passados para serem lidos, que pode mudar
   nao e exatamente igual ao limit, pode ser uma quantidade diferente.
*/

ITEM **reader_nasa_csv(const char *caminho_csv, int limit, int *total_lidos);
void reader_nasa_desalocar(ITEM ***vetor, int total_lidos);

#endif  