#include "auditoria.h"

static LogAuditoria g_logs[MAX_LOGS];
static int g_quantidade_logs = 0;
static int g_inicializado = 0;

void inicializar_auditoria(void) {
    if (g_inicializado) return;

    // Carrega dados de exemplo alinhados aos dados do front-end e dos testes originais
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
        // Desloca para o mais recente ficar no topo
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

int obter_quantidade_logs(void) {
    inicializar_auditoria();
    return g_quantidade_logs;
}

#define LOGS_POR_PAGINA 5

void mostrarLogs(int pagina) {
    inicializar_auditoria();
    LIMPAR_TELA();

    int total_paginas = (g_quantidade_logs + LOGS_POR_PAGINA - 1) / LOGS_POR_PAGINA;
    if (total_paginas == 0) total_paginas = 1;

    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | RELATORIO DE AUDITORIA DE OPERACOES (Pagina %d de %d)\n", pagina + 1, total_paginas);
    printf("================================================================================\n\n");

    printf("DATA/HORA        | USUARIO     | ACAO CRITICA       | DETALHES / MOTIVO\n");
    printf("--------------------------------------------------------------------------------\n");

    int inicio = pagina * LOGS_POR_PAGINA;
    int fim = inicio + LOGS_POR_PAGINA;
    if (fim > g_quantidade_logs) fim = g_quantidade_logs;

    for (int i = inicio; i < fim; i++) {
        printf("%-16s | %-11s | %-18s | %s\n",
            g_logs[i].datahora,
            g_logs[i].usuario,
            g_logs[i].acao,
            g_logs[i].detalhes);
    }

    if (g_quantidade_logs == 0) {
        printf(" Nenhum registro de auditoria no momento.\n");
    }

    printf("--------------------------------------------------------------------------------\n\n");
    printf("[P] Proxima Pagina  |  [A] Pagina Anterior  |  [F] Filtrar por Usuario  |  [0] Voltar\n");
    printf("Opcao > ");
}

void filtrarporusuario(void) {
    inicializar_auditoria();
    char usuariobusca[24];
    int encontrou = 0;

    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | FILTRO DE AUDITORIA\n");
    printf("================================================================================\n\n");

    printf("Digite o usuario a buscar: ");
    if (scanf("%23s", usuariobusca) != 1) {
        limpar_buffer();
        return;
    }
    limpar_buffer();

    printf("\nRESULTADOS PARA: %s\n\n", usuariobusca);
    printf("DATA/HORA        | USUARIO     | ACAO CRITICA       | DETALHES / MOTIVO\n");
    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < g_quantidade_logs; i++) {
        if (strstr(g_logs[i].usuario, usuariobusca) != NULL) {
            printf("%-16s | %-11s | %-18s | %s\n",
                   g_logs[i].datahora,
                   g_logs[i].usuario,
                   g_logs[i].acao,
                   g_logs[i].detalhes);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Nenhum registro encontrado para '%s'.\n", usuariobusca);
    }

    pausar_sistema();
}

EstadoSistema telaLogAuditoria(void) {
    inicializar_auditoria();
    int pagina = 0;
    char opcao[16];

    while (1) {
        int total_paginas = (g_quantidade_logs + LOGS_POR_PAGINA - 1) / LOGS_POR_PAGINA;
        if (total_paginas == 0) total_paginas = 1;

        mostrarLogs(pagina);
        if (scanf("%15s", opcao) != 1) {
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        if (opcao[0] == 'P' || opcao[0] == 'p') {
            if (pagina + 1 < total_paginas) {
                pagina++;
            } else {
                printf("\nNao ha mais paginas seguintes.\n");
                pausar_sistema();
            }
        } else if (opcao[0] == 'A' || opcao[0] == 'a') {
            if (pagina > 0) {
                pagina--;
            } else {
                printf("\nVoce ja esta na primeira pagina.\n");
                pausar_sistema();
            }
        } else if (opcao[0] == 'F' || opcao[0] == 'f') {
            filtrarporusuario();
        } else if (opcao[0] == '0') {
            return ESTADO_MENU_PRINCIPAL;
        } else {
            printf("\nOpcao invalida!\n");
            pausar_sistema();
        }
    }
}
