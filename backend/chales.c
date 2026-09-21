#include "chales.h"

static Chale g_chales[NUM_CHALES];
static int g_inicializado = 0;

const char *rotulo_status(StatusChale s) {
    switch (s) {
        case LIVRE:      return "[ LIVR ]";
        case RESERVADO:  return "[ RESV ]";
        case OCUPADO:    return "[ OCUP ]";
        case LIMPEZA:    return "[ LIMP ]";
        case MANUTENCAO: return "[ MANU ]";
        default:         return "[ ???? ]";
    }
}

const char *nome_status(StatusChale s) {
    switch (s) {
        case LIVRE:      return "Livre";
        case RESERVADO:  return "Reservado";
        case OCUPADO:    return "Ocupado";
        case LIMPEZA:    return "Limpeza";
        case MANUTENCAO: return "Manutenção";
        default:         return "Desconhecido";
    }
}

void inicializar_chales(void) {
    if (g_inicializado) return;

    // Dados de exemplo originais desenvolvidos por Carlos
    StatusChale s0[TOTAL_DIAS] = { OCUPADO, OCUPADO, LIVRE, LIVRE, RESERVADO, LIVRE, LIVRE,
                                   LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
    StatusChale s1[TOTAL_DIAS] = { LIMPEZA, LIVRE, RESERVADO, RESERVADO, RESERVADO, LIVRE, LIVRE,
                                   LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
    StatusChale s2[TOTAL_DIAS] = { LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE,
                                   LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
    StatusChale s3[TOTAL_DIAS] = { MANUTENCAO, MANUTENCAO, MANUTENCAO, LIVRE, LIVRE, LIVRE, LIVRE,
                                   LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };

    strcpy(g_chales[0].id, "CH-01"); strcpy(g_chales[0].tipo, "Luxo");
    memcpy(g_chales[0].status, s0, sizeof(s0));

    strcpy(g_chales[1].id, "CH-02"); strcpy(g_chales[1].tipo, "Familia");
    memcpy(g_chales[1].status, s1, sizeof(s1));

    strcpy(g_chales[2].id, "CH-03"); strcpy(g_chales[2].tipo, "Standard");
    memcpy(g_chales[2].status, s2, sizeof(s2));

    strcpy(g_chales[3].id, "CH-04"); strcpy(g_chales[3].tipo, "Standard");
    memcpy(g_chales[3].status, s3, sizeof(s3));

    g_inicializado = 1;
}

Chale* obter_chales(void) {
    inicializar_chales();
    return g_chales;
}

int obter_total_chales(void) {
    return NUM_CHALES;
}

int contar_chales_livres_hoje(void) {
    inicializar_chales();
    int livres = 0;
    for (int i = 0; i < NUM_CHALES; i++) {
        if (g_chales[i].status[0] == LIVRE) {
            livres++;
        }
    }
    return livres;
}

void atualizar_status_chale(const char *id_chale, int dia_offset, StatusChale novo_status) {
    inicializar_chales();
    for (int i = 0; i < NUM_CHALES; i++) {
        if (strcmp(g_chales[i].id, id_chale) == 0) {
            if (dia_offset >= 0 && dia_offset < TOTAL_DIAS) {
                g_chales[i].status[dia_offset] = novo_status;
            }
            break;
        }
    }
}

Chale* buscar_chale_por_id(const char *id_chale) {
    inicializar_chales();
    for (int i = 0; i < NUM_CHALES; i++) {
        if (strcmp(g_chales[i].id, id_chale) == 0) {
            return &g_chales[i];
        }
    }
    return NULL;
}
