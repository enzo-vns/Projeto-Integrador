#include "tela_auditoria.h"
#include "../backend/auditoria.h"

#define LOGS_POR_PAGINA 5

static void mostrar_logs_paginados(LogAuditoria logs[], int quantidade, int pagina) {
    LIMPAR_TELA();

    int total_paginas = (quantidade + LOGS_POR_PAGINA - 1) / LOGS_POR_PAGINA;
    if (total_paginas == 0) total_paginas = 1;

    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | RELATORIO DE AUDITORIA DE OPERACOES (Pagina %d de %d)\n", pagina + 1, total_paginas);
    printf("================================================================================\n\n");

    printf("DATA/HORA        | USUARIO     | ACAO CRITICA       | DETALHES / MOTIVO\n");
    printf("--------------------------------------------------------------------------------\n");

    int inicio = pagina * LOGS_POR_PAGINA;
    int fim = inicio + LOGS_POR_PAGINA;
    if (fim > quantidade) fim = quantidade;

    for (int i = inicio; i < fim; i++) {
        printf("%-16s | %-11s | %-18s | %s\n",
            logs[i].datahora,
            logs[i].usuario,
            logs[i].acao,
            logs[i].detalhes);
    }

    if (quantidade == 0) {
        printf(" Nenhum registro de auditoria encontrado.\n");
    }

    printf("--------------------------------------------------------------------------------\n\n");
    printf("[P] Proxima Pagina  |  [A] Pagina Anterior  |  [F] Filtrar por Usuario  |  [0] Voltar\n");
    printf("Opcao > ");
}

static void filtrar_por_usuario(LogAuditoria logs[], int quantidade) {
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

    for (int i = 0; i < quantidade; i++) {
        if (strstr(logs[i].usuario, usuariobusca) != NULL) {
            printf("%-16s | %-11s | %-18s | %s\n",
                   logs[i].datahora,
                   logs[i].usuario,
                   logs[i].acao,
                   logs[i].detalhes);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Nenhum registro encontrado para '%s'.\n", usuariobusca);
    }

    pausar_sistema();
}

EstadoSistema exibir_tela_auditoria(void) {
    LogAuditoria *logs = obter_logs_auditoria();
    int pagina = 0;
    char opcao[16];

    while (1) {
        int quantidade = obter_total_logs_auditoria();
        int total_paginas = (quantidade + LOGS_POR_PAGINA - 1) / LOGS_POR_PAGINA;
        if (total_paginas == 0) total_paginas = 1;

        mostrar_logs_paginados(logs, quantidade, pagina);
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
            filtrar_por_usuario(logs, quantidade);
        } else if (opcao[0] == '0') {
            return ESTADO_MENU_PRINCIPAL;
        } else {
            printf("\nOpcao invalida!\n");
            pausar_sistema();
        }
    }
}
