#ifndef HOSPEDES_H
#define HOSPEDES_H

#include "../common/common.h"

#define MAX_HOSPEDES 50

typedef struct {
    int id;
    char nome[50];
    char documento[20];
    char telefone[20];
} Hospede;

void inicializar_hospedes(void);
Hospede* obter_hospedes(void);
int obter_total_hospedes(void);
int cadastrar_hospede(const char *nome, const char *documento, const char *telefone);
Hospede* buscar_hospede_por_id(int id);

#endif // HOSPEDES_H
