#ifndef CHALES_H
#define CHALES_H

#include "common.h"

#define NUM_CHALES     4
#define DIAS_VISIVEIS  5   
#define TOTAL_DIAS     14  

typedef enum {
    LIVRE,
    RESERVADO,
    OCUPADO,
    LIMPEZA,
    MANUTENCAO
} StatusChale;

typedef struct {
    char id[8];
    char tipo[16];
    StatusChale status[TOTAL_DIAS];
} Chale;

// Inicialização e acesso
void inicializar_chales(void);
Chale* obter_chales(void);
int obter_total_chales(void);
int contar_chales_livres_hoje(void);
const char *rotulo_status(StatusChale s);
const char *nome_status(StatusChale s);
void atualizar_status_chale(const char *id_chale, int dia_offset, StatusChale novo_status);

// Tela interativa (desenvolvida por Carlos)
EstadoSistema tela_mapa_ocupacao(void);

#endif // CHALES_H
