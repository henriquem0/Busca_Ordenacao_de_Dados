#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estrutura.h"
#include "tads/pilha.h"
#include "tads/fila.h"
#include "tads/lista_seq.h"
#include "tads/lista_enc.h"
#include "tads/lista_cabeca.h"
#include "tads/lista_ord.h"
#include "tads/lista_gen.h"
#include "tads/lista_cruz.h"

// nome usado como grupo quando o item nao tem valor no campo de agrupamento
#define VALOR_AUSENTE "(vazio)"

struct estrutura_{
    TipoEstrutura tipo;
    bool dono_dos_itens;
    const QUERY *query;
    int posicao; // posicao do percurso nas estruturas de vetor (pilha, fila, lista_seq)
    union{
        PILHA *pilha;
        FILA *fila;
        LISTA_SEQ *lista_seq;
        LISTA_ENC *lista_enc;
        LISTA_CABECA *lista_cabeca;
        LISTA_ORD *lista_ord;
        LISTA_GEN *lista_gen;
        LISTA_CRUZ *lista_cruz;
    } dados;
};

// nomes aceitos em --base e --result, na mesma ordem do enum
static const char *NOMES_ESTRUTURAS[] = {
    "pilha", "fila", "lista_seq", "lista_enc",
    "lista_cabeca", "lista_ord", "lista_gen", "lista_cruz"
};

bool estrutura_tipo_de_texto(const char *texto, TipoEstrutura *tipo){
    if(texto == NULL || tipo == NULL){
        return false;
    }
    for(int i = 0; i < 8; i++){
        if(strcmp(texto, NOMES_ESTRUTURAS[i]) == 0){ // verifica se o texto corresponde a uma estrutura
            *tipo = (TipoEstrutura)i;
            return true;
        }
    }
    return false;
}

// valor do campo como texto para usar como nome de grupo (ou valor_ausente)
static const char *valor_do_campo(const ITEM *item, const char *campo){
    const char *valor = NULL;
    if(campo != NULL && campo[0] != '\0'){
        valor = item_get_campo(item, campo);
    }
    if(valor == NULL){
        return VALOR_AUSENTE;
    }
    return valor;
}

// chave de ordem da lista_ord: a similaridade (padrao) ou o valor numerico do campo escolhido no order.
static double chave_de_ordem(const ESTRUTURA *e, const ITEM *item, double similaridade){
    const char *campo = query_get_campo_order(e->query);
    if(campo[0] == '\0' || strcmp(campo, "similarity") == 0){
        return similaridade;
    }
    double valor = 0.0;
    item_get_campo_numerico(item, campo, &valor);
    return valor;
}

ESTRUTURA *estrutura_criar(TipoEstrutura tipo, bool dono_dos_itens, const QUERY *query){
    ESTRUTURA *e = (ESTRUTURA *)malloc(sizeof(ESTRUTURA)); // reserva o descritor da estrutura
    if(e == NULL){
        return NULL;
    }
    e->tipo = tipo; // guarda o tipo para escolher as operacoes depois
    e->dono_dos_itens = dono_dos_itens;
    e->query = query;
    e->posicao = 0;
    e->dados.pilha = NULL;

    switch(tipo){ // cria o tad correspondente ao tipo recebido
        case ESTRUTURA_PILHA:       
            e->dados.pilha = pilha_criar();
            break;

        case ESTRUTURA_FILA:        
            e->dados.fila = fila_criar();
            break;

        case ESTRUTURA_LISTA_SEQ:   
            e->dados.lista_seq = lista_seq_criar(); 
            break;

        case ESTRUTURA_LISTA_ENC:   
            e->dados.lista_enc = lista_enc_criar(); 
            break;

        case ESTRUTURA_LISTA_CABECA:
            e->dados.lista_cabeca = lista_cabeca_criar(); 
            break;

        case ESTRUTURA_LISTA_ORD:   
            e->dados.lista_ord = lista_ord_criar(query_order_decrescente(query)); 
            break;

        case ESTRUTURA_LISTA_GEN:   
            e->dados.lista_gen = lista_gen_criar(); 
            break;

        case ESTRUTURA_LISTA_CRUZ:  
            e->dados.lista_cruz = lista_cruz_criar(); 
            break;
    }
    // todos os ponteiros do union ocupam o mesmo lugar, entao basta testar um if
    if(e->dados.pilha == NULL){
        free(e);
        return NULL;
    }
    return e;
}

// apaga o tad de baixo (com ou sem os itens, conforme o dono) e depois a capa
void estrutura_apagar(ESTRUTURA **e){
    if(e == NULL || *e == NULL){
        return;
    }
    ESTRUTURA *estrutura = *e;
    bool apagar_itens = estrutura->dono_dos_itens;

    switch(estrutura->tipo){
        case ESTRUTURA_PILHA:
            if(apagar_itens) pilha_apagar(&estrutura->dados.pilha);
            else pilha_apagar_sem_itens(&estrutura->dados.pilha);
            break;

        case ESTRUTURA_FILA:
            if(apagar_itens) fila_apagar(&estrutura->dados.fila);
            else fila_apagar_sem_itens(&estrutura->dados.fila);
            break;

        case ESTRUTURA_LISTA_SEQ:
            if(apagar_itens) lista_seq_apagar(&estrutura->dados.lista_seq);
            else lista_seq_apagar_sem_itens(&estrutura->dados.lista_seq);
            break;

        case ESTRUTURA_LISTA_ENC:
            if(apagar_itens) lista_enc_apagar(&estrutura->dados.lista_enc);
            else lista_enc_apagar_sem_itens(&estrutura->dados.lista_enc);
            break;

        case ESTRUTURA_LISTA_CABECA:
            if(apagar_itens) lista_cabeca_apagar(&estrutura->dados.lista_cabeca);
            else lista_cabeca_apagar_sem_itens(&estrutura->dados.lista_cabeca);
            break;
            
        case ESTRUTURA_LISTA_ORD:
            if(apagar_itens) lista_ord_apagar(&estrutura->dados.lista_ord);
            else lista_ord_apagar_sem_itens(&estrutura->dados.lista_ord);
            break;

        case ESTRUTURA_LISTA_GEN:
            if(apagar_itens) lista_gen_apagar(&estrutura->dados.lista_gen);
            else lista_gen_apagar_sem_itens(&estrutura->dados.lista_gen);
            break;

        case ESTRUTURA_LISTA_CRUZ:
            if(apagar_itens) lista_cruz_apagar(&estrutura->dados.lista_cruz);
            else lista_cruz_apagar_sem_itens(&estrutura->dados.lista_cruz);
            break;
    }
    free(estrutura);
    *e = NULL;
}

bool estrutura_inserir(ESTRUTURA *e, ITEM *item, double similaridade){
    if(e == NULL || item == NULL){
        return false;
    }
    switch(e->tipo){ // encaminha a insercao para o tad selecionado
        case ESTRUTURA_PILHA:
            return pilha_empilhar(e->dados.pilha, item);

        case ESTRUTURA_FILA:
            return fila_inserir(e->dados.fila, item);

        case ESTRUTURA_LISTA_SEQ:
            return lista_seq_inserir(e->dados.lista_seq, item);

        case ESTRUTURA_LISTA_ENC:
            return lista_enc_inserir(e->dados.lista_enc, item);

        case ESTRUTURA_LISTA_CABECA:
            return lista_cabeca_inserir(e->dados.lista_cabeca, item);
            
        case ESTRUTURA_LISTA_ORD:
            return lista_ord_inserir(e->dados.lista_ord, item, chave_de_ordem(e, item, similaridade));

        case ESTRUTURA_LISTA_GEN:
            return lista_gen_inserir(e->dados.lista_gen, item,
                                     valor_do_campo(item, query_get_campo_group(e->query, 1)),
                                     valor_do_campo(item, query_get_campo_group(e->query, 2)));

        case ESTRUTURA_LISTA_CRUZ:
            return lista_cruz_inserir(e->dados.lista_cruz, item,
                                      valor_do_campo(item, query_get_campo_cross(e->query, 1)),
                                      valor_do_campo(item, query_get_campo_cross(e->query, 2)));
    }
    return false;
}

int estrutura_tamanho(const ESTRUTURA *e){
    if(e == NULL){
        return 0;
    }
    switch(e->tipo){ // encaminha a remocao para o tad selecionado
        case ESTRUTURA_PILHA:        
            return pilha_tamanho(e->dados.pilha);

        case ESTRUTURA_FILA:         
            return fila_tamanho(e->dados.fila);

        case ESTRUTURA_LISTA_SEQ:    
            return lista_seq_tamanho(e->dados.lista_seq);

        case ESTRUTURA_LISTA_ENC:    
            return lista_enc_tamanho(e->dados.lista_enc);

        case ESTRUTURA_LISTA_CABECA: 
            return lista_cabeca_tamanho(e->dados.lista_cabeca);

        case ESTRUTURA_LISTA_ORD:    
            return lista_ord_tamanho(e->dados.lista_ord);

        case ESTRUTURA_LISTA_GEN:    
            return lista_gen_tamanho(e->dados.lista_gen);

        case ESTRUTURA_LISTA_CRUZ:   
            return lista_cruz_tamanho(e->dados.lista_cruz);
    }
    return 0;
}

void estrutura_reiniciar(ESTRUTURA *e){
    if(e == NULL){
        return;
    }
    switch(e->tipo){ // retorna o tamanho mantido pelo tad selecionado
        case ESTRUTURA_PILHA:        
            e->posicao = pilha_tamanho(e->dados.pilha) - 1;
            break; // comeca no topo

        case ESTRUTURA_FILA:         
            e->posicao = 0;
            break;

        case ESTRUTURA_LISTA_SEQ:    
            e->posicao = 0; 
            break;

        case ESTRUTURA_LISTA_ENC:    
            lista_enc_reiniciar(e->dados.lista_enc); 
            break;

        case ESTRUTURA_LISTA_CABECA: 
            lista_cabeca_reiniciar(e->dados.lista_cabeca); 
            break;

        case ESTRUTURA_LISTA_ORD:    
            lista_ord_reiniciar(e->dados.lista_ord); 
            break;

        case ESTRUTURA_LISTA_GEN:    
            lista_gen_reiniciar(e->dados.lista_gen); 
            break;

        case ESTRUTURA_LISTA_CRUZ:   
            lista_cruz_reiniciar(e->dados.lista_cruz, LISTA_CRUZ_EIXO_A); 
            break;
    }
}

ITEM *estrutura_proximo(ESTRUTURA *e){
    if(e == NULL){
        return NULL;
    }
    switch(e->tipo){
        case ESTRUTURA_PILHA:
            if(e->posicao < 0){
                return NULL;
            }
            return pilha_obter(e->dados.pilha, e->posicao--);



        case ESTRUTURA_FILA:
            return fila_obter(e->dados.fila, e->posicao++);

        case ESTRUTURA_LISTA_SEQ:
            return lista_seq_obter(e->dados.lista_seq, e->posicao++);

        case ESTRUTURA_LISTA_ENC:    
            return lista_enc_proximo(e->dados.lista_enc);

        case ESTRUTURA_LISTA_CABECA: 
            return lista_cabeca_proximo(e->dados.lista_cabeca);

        case ESTRUTURA_LISTA_ORD:    
            return lista_ord_proximo(e->dados.lista_ord);

        case ESTRUTURA_LISTA_GEN:    
            return lista_gen_proximo(e->dados.lista_gen);

        case ESTRUTURA_LISTA_CRUZ:   
            return lista_cruz_proximo(e->dados.lista_cruz, LISTA_CRUZ_EIXO_A);
    }
    return NULL;
}

// remove o item da frente. nas listas, pega o primeiro item pelo percurso e remove pelo identificador dele (o primeiro com esso identificador e ele mesmo).
ITEM *estrutura_remover(ESTRUTURA *e){
    if(e == NULL){
        return NULL;
    }
    if(e->tipo == ESTRUTURA_PILHA){
        return pilha_desempilhar(e->dados.pilha);
    }
    if(e->tipo == ESTRUTURA_FILA){
        return fila_remover(e->dados.fila);
    }
    if(e->tipo == ESTRUTURA_LISTA_GEN || e->tipo == ESTRUTURA_LISTA_CRUZ){
        return NULL; // remocao nao implementada nessas listas
    }

    estrutura_reiniciar(e);
    ITEM *primeiro = estrutura_proximo(e);
    if(primeiro == NULL){
        return NULL;
    }
    const char *identificador = item_get_identificador(primeiro);
    switch(e->tipo){
        case ESTRUTURA_LISTA_SEQ:    
            return lista_seq_remover(e->dados.lista_seq, identificador);

        case ESTRUTURA_LISTA_ENC:    
            return lista_enc_remover(e->dados.lista_enc, identificador);
            

        case ESTRUTURA_LISTA_CABECA: 
            return lista_cabeca_remover(e->dados.lista_cabeca, identificador);

        case ESTRUTURA_LISTA_ORD:    
            return lista_ord_remover(e->dados.lista_ord, identificador);


        default:                     
            return NULL;
    }
}
