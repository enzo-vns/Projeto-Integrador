#ifndef AUDITORIA_H
#define AUDITORIA_H

#include "../common/common.h"

#define MAX_LOGS 100

// Estrutura de Log de Auditoria desenvolvida originalmente por Rayane
typedef struct {
    char datahora[24];
    char usuario[24];
    char acao[32];
    char detalhes[80];
} LogAuditoria;

// Inicializa a base de auditoria com os registros pré-carregados
void inicializar_auditoria(void);

// Registra uma nova operação crítica na auditoria
void adicionar_log_auditoria(const char *usuario, const char *acao, const char *detalhes);

// Retorna o ponteiro para o array de logs
LogAuditoria* obter_logs_auditoria(void);

// Retorna a quantidade total de registros gravados
int obter_total_logs_auditoria(void);

#endif // AUDITORIA_H
