#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LOGS 100

typedef struct {
    char datahora[20];
    char usuario [20];
    char acao[25];
    char detalhes[60];

} LogAuditoria;

void limpartela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void mostrarLogs(LogAuditoria logs[], int quantidade) {
    int i;

    limpartela();

    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | RELATORIO DE AUDITORIA DE OPERACOES\n");
    printf("================================================================================\n\n");

    printf("DATA/HORA        | USUARIO     | ACAO CRITICA       | DETALHES / MOTIVO\n");
    printf("--------------------------------------------------------------------------------\n");

    for(i = 0; i < quantidade; i++) {
        printf("%-16s | %-11s | %-18s | %s\n",
        logs[i].datahora,
        logs[i].usuario,
        logs[i].acao,
        logs[i].detalhes);
    }
    printf("--------------------------------------------------------------------------------\n\n");

    printf("[P] Proxima Pagina  |  [A] Pagina Anterior  |  [F] Filtrar por Usuario  |  [0] Voltar\n");
    printf("Opcao > ");
}

void filtrarporusuario(LogAuditoria logs[], int quantidade) {
    char usuariobusca[20];
    int encontrou = 0;

    limpartela();

    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | FILTRO DE AUDITORIA\n");
    printf("================================================================================\n\n");

    printf("Digite o usuario: ");
    scanf("%19s", usuariobusca);

    printf("\nRESULTADOS PARA: %s\n\n", usuariobusca);

    printf("DATA/HORA        | USUARIO     | ACAO CRITICA       | DETALHES / MOTIVO\n");
    printf("--------------------------------------------------------------------------------\n");

    for(int i = 0; i < quantidade; i++) {
        if(strcmp(logs[i].usuario, usuariobusca) == 0) {
            printf("%-16s | %-11s | %-18s | %s\n",
                   logs[i].datahora,
                   logs[i].usuario,
                   logs[i].acao,
                   logs[i].detalhes);

            encontrou = 1;
        }
    }

    if(!encontrou) {
        printf("Nenhum registro encontrado.\n");
    }

    printf("\nPressione ENTER para voltar...");
    getchar();
    getchar();
}

void telaLogAuditoria() {
    LogAuditoria logs[MAX_LOGS] = {
        {
            "20/09/2026 18:36",
            "ana.gov",
            "STATUS_CHALE",
            "CH-02 alterado para LIMPEZA",
        },

        {
            "20/09/2026 21:00",
            "marcos.rec",
            "CHECKIN_REALIZADO",
            "Reserva #87 (Hospede: Carlos)"
        },

        {
            "20/09/2026 21:43",
            "marcos.rec",
            "CANCELA_RESERVA",
            "Reserva #97 - Motivo: Desistencia"
        }
    };

    int quantidade = 3;
    char opcao;

    do {
        mostrarLogs(logs, quantidade);
        scanf("%c", &opcao);

        switch(opcao) {
            case 'P':
            case 'p':
                printf("\nNao ha mais paginas.\n");
                printf("Pressione ENTER para continuar...");
                getchar();
                getchar();
                break;

            case 'A':
            case 'a':
                printf("\nVoce ja esta na primeira pagina.\n");
                printf("Pressione ENTER para continuar...");
                getchar();
                getchar();
                break;

            case 'F':
            case 'f':
                filtrarporusuario(logs, quantidade);
                break;

            case '0':
                printf("\nVoltando ao menu principal...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
                printf("Pressione ENTER para continuar...");
                getchar();
                getchar();
        }

    } while (opcao != '0');
}