#ifndef RESERVAS_H
#define RESERVAS_H

#include "common.h"
#include "chales.h"

#define MAX_RESERVAS 50

typedef struct {
    int id;
    char hospede[50];
    char chale[8];
    char checkin[12];
    char checkout[12];
    char status[16];
    int checkedIn;
    int checkedOut;
} Reserva;

void inicializar_reservas(void);
Reserva* obter_reservas(void);
int obter_total_reservas(void);

int contar_checkins_pendentes(void);
int contar_checkouts_pendentes(void);

int criar_reserva(const char *hospede, const char *chale, const char *checkin, const char *checkout);
int efetuar_checkin(int id_reserva);
int efetuar_checkout(int id_reserva);
int cancelar_reserva(int id_reserva);

Reserva* buscar_reserva_por_id(int id);

#endif // RESERVAS_H
