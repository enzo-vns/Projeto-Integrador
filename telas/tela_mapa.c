#include "tela_mapa.h"
#include "../backend/chales.h"

static const char *dia_semana_abrev(int wday) {
    static const char *dias[] = { "Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab" };
    return dias[wday % 7];
}

static void imprimir_cabecalho(time_t data_inicial) {
    struct tm dia = *localtime(&data_inicial);

    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | MAPA DE OCUPACAO DA SEMANA                                   \n");
    printf("================================================================================\n\n");

    printf("%-8s| %-9s", "Chale", "Tipo");
    for (int i = 0; i < DIAS_VISIVEIS; i++) {
        printf("| %s %02d/%02d ", dia_semana_abrev(dia.tm_wday), dia.tm_mday, dia.tm_mon + 1);
        dia.tm_mday++;
        mktime(&dia); 
    }
    printf("\n--------------------------------------------------------------------------------\n");
}

static void imprimir_linha_chale(const Chale *c, int offset) {
    printf("%-8s| %-9s", c->id, c->tipo);
    for (int i = 0; i < DIAS_VISIVEIS; i++) {
        int idx = offset + i;
        StatusChale s = (idx < TOTAL_DIAS) ? c->status[idx] : LIVRE;
        printf("| %s ", rotulo_status(s));
    }
    printf("\n");
}

static void imprimir_legenda(void) {
    printf("--------------------------------------------------------------------------------\n");
    printf("Legenda: [LIVR] Livre | [RESV] Reservado | [OCUP] Ocupado | [LIMP] Limpeza | [MANU] Manutencao\n\n");
}

EstadoSistema exibir_tela_mapa(void) {
    Chale *chales = obter_chales();
    int total_chales = obter_total_chales();

    time_t data_inicial = time(NULL);
    int offset_dias = 0;
    int opcao;

    while (1) {
        LIMPAR_TELA();

        imprimir_cabecalho(data_inicial);
        for (int i = 0; i < total_chales; i++) {
            imprimir_linha_chale(&chales[i], offset_dias);
        }
        imprimir_legenda();

        printf("[1] Avancar 7 dias   [2] Voltar 7 dias   [0] Voltar ao Menu\n");
        printf("Opcao > ");

        opcao = ler_opcao_segura();

        switch (opcao) {
            case 1:
                if (offset_dias + 7 < TOTAL_DIAS) {
                    offset_dias += 7;
                    data_inicial += 7 * 24 * 60 * 60;
                } else {
                    printf("\nLimite maximo de dias alcancado.\n");
                    pausar_sistema();
                }
                break;
            case 2:
                if (offset_dias >= 7) {
                    offset_dias -= 7;
                    data_inicial -= 7 * 24 * 60 * 60;
                } else {
                    printf("\nVoce ja esta visualizando a data de hoje.\n");
                    pausar_sistema();
                }
                break;
            case 0:
                return ESTADO_MENU_PRINCIPAL;
            default:
                printf("\nOpcao invalida! Pressione ENTER para continuar...");
                getchar();
        }
    }
}
