#include "hospedes.h"

static Hospede g_hospedes[MAX_HOSPEDES];
static int g_total_hospedes = 0;
static int g_inicializado = 0;

void inicializar_hospedes(void) {
    if (g_inicializado) return;

    g_hospedes[0] = (Hospede){ 1, "Carlos Andrade", "321.654.987-00", "(11) 98888-1122" };
    g_hospedes[1] = (Hospede){ 2, "Beatriz Lima",   "112.334.556-70", "(11) 97777-3344" };
    g_hospedes[2] = (Hospede){ 3, "Renato Souza",   "998.877.665-40", "(11) 96666-5566" };
    g_total_hospedes = 3;

    g_inicializado = 1;
}

Hospede* obter_hospedes(void) {
    inicializar_hospedes();
    return g_hospedes;
}

int obter_total_hospedes(void) {
    inicializar_hospedes();
    return g_total_hospedes;
}

int cadastrar_hospede(const char *nome, const char *documento, const char *telefone) {
    inicializar_hospedes();

    if (g_total_hospedes >= MAX_HOSPEDES) {
        return -1; // Limite atingido
    }

    Hospede novo;
    novo.id = g_total_hospedes + 1;
    strncpy(novo.nome, nome, sizeof(novo.nome) - 1);
    novo.nome[sizeof(novo.nome) - 1] = '\0';

    strncpy(novo.documento, documento, sizeof(novo.documento) - 1);
    novo.documento[sizeof(novo.documento) - 1] = '\0';

    strncpy(novo.telefone, telefone, sizeof(novo.telefone) - 1);
    novo.telefone[sizeof(novo.telefone) - 1] = '\0';

    g_hospedes[g_total_hospedes] = novo;
    g_total_hospedes++;

    return novo.id;
}

Hospede* buscar_hospede_por_id(int id) {
    inicializar_hospedes();
    for (int i = 0; i < g_total_hospedes; i++) {
        if (g_hospedes[i].id == id) {
            return &g_hospedes[i];
        }
    }
    return NULL;
}
