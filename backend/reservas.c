#include "reservas.h"
#include "chales.h"

static Reserva g_reservas[MAX_RESERVAS];
static int g_total_reservas = 0;
static int g_inicializado = 0;

void inicializar_reservas(void) {
    if (g_inicializado) return;

    inicializar_chales();

    g_reservas[0] = (Reserva){ 104, "Carlos Andrade", "CH-01", "08/09/2026", "10/09/2026", "confirmada", 0, 0 };
    g_reservas[1] = (Reserva){ 105, "Beatriz Lima",   "CH-02", "10/09/2026", "13/09/2026", "confirmada", 0, 0 };
    g_reservas[2] = (Reserva){ 98,  "Renato Souza",   "CH-03", "05/09/2026", "07/09/2026", "cancelada",  0, 0 };
    g_total_reservas = 3;

    g_inicializado = 1;
}

Reserva* obter_reservas(void) {
    inicializar_reservas();
    return g_reservas;
}

int obter_total_reservas(void) {
    inicializar_reservas();
    return g_total_reservas;
}

int contar_checkins_pendentes(void) {
    inicializar_reservas();
    int count = 0;
    for (int i = 0; i < g_total_reservas; i++) {
        if (strcmp(g_reservas[i].status, "confirmada") == 0 && !g_reservas[i].checkedIn) {
            count++;
        }
    }
    return count;
}

int contar_checkouts_pendentes(void) {
    inicializar_reservas();
    int count = 0;
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].checkedIn && !g_reservas[i].checkedOut) {
            count++;
        }
    }
    return count;
}

int criar_reserva(const char *hospede, const char *chale, const char *checkin, const char *checkout) {
    inicializar_reservas();

    if (g_total_reservas >= MAX_RESERVAS) {
        return -1;
    }

    Reserva nova;
    nova.id = 100 + g_total_reservas + 1;
    strncpy(nova.hospede, hospede, sizeof(nova.hospede) - 1);
    nova.hospede[sizeof(nova.hospede) - 1] = '\0';

    strncpy(nova.chale, chale, sizeof(nova.chale) - 1);
    nova.chale[sizeof(nova.chale) - 1] = '\0';

    strncpy(nova.checkin, checkin, sizeof(nova.checkin) - 1);
    nova.checkin[sizeof(nova.checkin) - 1] = '\0';

    strncpy(nova.checkout, checkout, sizeof(nova.checkout) - 1);
    nova.checkout[sizeof(nova.checkout) - 1] = '\0';

    strcpy(nova.status, "confirmada");
    nova.checkedIn = 0;
    nova.checkedOut = 0;

    g_reservas[g_total_reservas++] = nova;
    atualizar_status_chale(nova.chale, 0, RESERVADO);

    return nova.id;
}

int efetuar_checkin(int id_reserva) {
    inicializar_reservas();
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].id == id_reserva && !g_reservas[i].checkedIn) {
            g_reservas[i].checkedIn = 1;
            atualizar_status_chale(g_reservas[i].chale, 0, OCUPADO);
            return 1;
        }
    }
    return 0;
}

int efetuar_checkout(int id_reserva) {
    inicializar_reservas();
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].id == id_reserva && g_reservas[i].checkedIn && !g_reservas[i].checkedOut) {
            g_reservas[i].checkedOut = 1;
            atualizar_status_chale(g_reservas[i].chale, 0, LIMPEZA);
            return 1;
        }
    }
    return 0;
}

int cancelar_reserva(int id_reserva) {
    inicializar_reservas();
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].id == id_reserva) {
            if (strcmp(g_reservas[i].status, "cancelada") == 0) {
                return -1; // já cancelada
            }
            strcpy(g_reservas[i].status, "cancelada");
            atualizar_status_chale(g_reservas[i].chale, 0, LIVRE);
            return 1;
        }
    }
    return 0;
}

Reserva* buscar_reserva_por_id(int id) {
    inicializar_reservas();
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].id == id) {
            return &g_reservas[i];
        }
    }
    return NULL;
}
