#ifndef AUDITORIA_H
#define AUDITORIA_H

#include "common.h"

#define MAX_LOGS 100

typedef struct {
    char datahora[24];
    char usuario[24];
    char acao[32];
    char detalhes[80];
} LogAuditoria;

// Gerenciamento e acesso
void inicializar_auditoria(void);
void adicionar_log_auditoria(const char *usuario, const char *acao, const char *detalhes);
int obter_quantidade_logs(void);

// Telas de auditoria (desenvolvidas por Rayane)
void mostrarLogs(int pagina);
void filtrarporusuario(void);
EstadoSistema telaLogAuditoria(void);

#endif // AUDITORIA_H
