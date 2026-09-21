#include "auditoria.h"

static LogAuditoria g_logs[MAX_LOGS];
static int g_quantidade_logs = 0;
static int g_inicializado = 0;

void inicializar_auditoria(void) {
    if (g_inicializado) return;

    // Registros iniciais baseados nos dados originais da Rayane
    strcpy(g_logs[0].datahora, "20/09/2026 18:36");
    strcpy(g_logs[0].usuario,  "ana.gov");
    strcpy(g_logs[0].acao,     "STATUS_CHALE");
    strcpy(g_logs[0].detalhes, "CH-02 alterado para LIMPEZA");

    strcpy(g_logs[1].datahora, "20/09/2026 21:00");
    strcpy(g_logs[1].usuario,  "marcos.rec");
    strcpy(g_logs[1].acao,     "CHECKIN_REALIZADO");
    strcpy(g_logs[1].detalhes, "Reserva #87 (Hospede: Carlos)");

    strcpy(g_logs[2].datahora, "20/09/2026 21:43");
    strcpy(g_logs[2].usuario,  "marcos.rec");
    strcpy(g_logs[2].acao,     "CANCELA_RESERVA");
    strcpy(g_logs[2].detalhes, "Reserva #97 - Motivo: Desistencia");

    g_quantidade_logs = 3;
    g_inicializado = 1;
}

void adicionar_log_auditoria(const char *usuario, const char *acao, const char *detalhes) {
    inicializar_auditoria();

    if (g_quantidade_logs < MAX_LOGS) {
        // Desloca os logs existentes para abrir a primeira posição para o registro mais recente
        for (int i = g_quantidade_logs; i > 0; i--) {
            g_logs[i] = g_logs[i - 1];
        }
        obter_data_hora_atual(g_logs[0].datahora, sizeof(g_logs[0].datahora));
        strncpy(g_logs[0].usuario, usuario ? usuario : "sistema", sizeof(g_logs[0].usuario) - 1);
        strncpy(g_logs[0].acao, acao ? acao : "OPERACAO", sizeof(g_logs[0].acao) - 1);
        strncpy(g_logs[0].detalhes, detalhes ? detalhes : "", sizeof(g_logs[0].detalhes) - 1);
        g_quantidade_logs++;
    }
}

LogAuditoria* obter_logs_auditoria(void) {
    inicializar_auditoria();
    return g_logs;
}

int obter_total_logs_auditoria(void) {
    inicializar_auditoria();
    return g_quantidade_logs;
}
