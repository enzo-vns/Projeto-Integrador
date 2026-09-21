#ifndef CHALES_H
#define CHALES_H

#include "common.h"

#define NUM_CHALES     4
#define DIAS_VISIVEIS  5   
#define TOTAL_DIAS     14  

// Estados dos chalés (desenvolvido por Carlos)
typedef enum {
    LIVRE,
    RESERVADO,
    OCUPADO,
    LIMPEZA,
    MANUTENCAO
} StatusChale;

// Estrutura do chalé com controle temporal de ocupação (Carlos)
typedef struct {
    char id[8];
    char tipo[16];
    StatusChale status[TOTAL_DIAS];
} Chale;

// Funções de inicialização e consulta
void inicializar_chales(void);
Chale* obter_chales(void);
int obter_total_chales(void);
int contar_chales_livres_hoje(void);

// Rótulos visuais para tabela ASCII
const char *rotulo_status(StatusChale s);
const char *nome_status(StatusChale s);

// Atualização de status
void atualizar_status_chale(const char *id_chale, int dia_offset, StatusChale novo_status);
Chale* buscar_chale_por_id(const char *id_chale);

#endif // CHALES_H
