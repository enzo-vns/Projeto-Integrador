#include "consumo.h"

static ItemConsumo g_consumos[MAX_CONSUMO];
static int g_total_consumos = 0;
static int g_inicializado = 0;

void inicializar_consumo(void) {
    if (g_inicializado) return;
    g_total_consumos = 0;
    g_inicializado = 1;
}

int lancar_consumo(const char *chale, const char *item, float valor) {
    inicializar_consumo();

    if (g_total_consumos >= MAX_CONSUMO) {
        return -1;
    }

    ItemConsumo novo;
    novo.id = g_total_consumos + 1;
    strncpy(novo.chale, chale, sizeof(novo.chale) - 1);
    novo.chale[sizeof(novo.chale) - 1] = '\0';

    strncpy(novo.item, item, sizeof(novo.item) - 1);
    novo.item[sizeof(novo.item) - 1] = '\0';

    novo.valor = valor;
    obter_data_hora_atual(novo.datahora, sizeof(novo.datahora));

    g_consumos[g_total_consumos++] = novo;
    return novo.id;
}

ItemConsumo* obter_historico_consumo(void) {
    inicializar_consumo();
    return g_consumos;
}

int obter_total_consumos(void) {
    inicializar_consumo();
    return g_total_consumos;
}
