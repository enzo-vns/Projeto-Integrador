#ifndef CONSUMO_H
#define CONSUMO_H

#include "common.h"

#define MAX_CONSUMO 100

typedef struct {
    int id;
    char chale[8];
    char item[50];
    float valor;
    char datahora[24];
} ItemConsumo;

void inicializar_consumo(void);
int lancar_consumo(const char *chale, const char *item, float valor);
ItemConsumo* obter_historico_consumo(void);
int obter_total_consumos(void);

#endif // CONSUMO_H
