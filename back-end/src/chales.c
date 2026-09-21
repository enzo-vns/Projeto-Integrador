#include "chales.h"

static Chale g_chales[NUM_CHALES];
static int g_inicializado = 0;

const char *rotulo_status(StatusChale s) {
    switch (s) {
        case LIVRE:      return "[ LIVR ]";
        case RESERVADO:  return "[ RESV ]";
        case OCUPADO:    return "[ OCUP ]";
        case LIMPEZA:    return "[ LIMP ]";
        case MANUTENCAO: return "[ MANU ]";
        default:         return "[ ???? ]";
    }
}

const char *nome_status(StatusChale s) {
    switch (s) {
        case LIVRE:      return "Livre";
        case RESERVADO:  return "Reservado";
        case OCUPADO:    return "Ocupado";
        case LIMPEZA:    return "Limpeza";
        case MANUTENCAO: return "Manutenção";
        default:         return "Desconhecido";
    }
}

static const char *dia_semana_abrev(int wday) {
    static const char *dias[] = { "Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab" };
    return dias[wday % 7];
}

void inicializar_chales(void) {
    if (g_inicializado) return;

    StatusChale s0[TOTAL_DIAS] = { OCUPADO, OCUPADO, LIVRE, LIVRE, RESERVADO, LIVRE, LIVRE,
                                   LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
    StatusChale s1[TOTAL_DIAS] = { LIMPEZA, LIVRE, RESERVADO, RESERVADO, RESERVADO, LIVRE, LIVRE,
                                   LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
    StatusChale s2[TOTAL_DIAS] = { LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE,
                                   LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
    StatusChale s3[TOTAL_DIAS] = { MANUTENCAO, MANUTENCAO, MANUTENCAO, LIVRE, LIVRE, LIVRE, LIVRE,
                                   LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };

    strcpy(g_chales[0].id, "CH-01"); strcpy(g_chales[0].tipo, "Luxo");
    memcpy(g_chales[0].status, s0, sizeof(s0));

    strcpy(g_chales[1].id, "CH-02"); strcpy(g_chales[1].tipo, "Familia");
    memcpy(g_chales[1].status, s1, sizeof(s1));

    strcpy(g_chales[2].id, "CH-03"); strcpy(g_chales[2].tipo, "Standard");
    memcpy(g_chales[2].status, s2, sizeof(s2));

    strcpy(g_chales[3].id, "CH-04"); strcpy(g_chales[3].tipo, "Standard");
    memcpy(g_chales[3].status, s3, sizeof(s3));

    g_inicializado = 1;
}

Chale* obter_chales(void) {
    inicializar_chales();
    return g_chales;
}

int obter_total_chales(void) {
    return NUM_CHALES;
}

int contar_chales_livres_hoje(void) {
    inicializar_chales();
    int livres = 0;
    for (int i = 0; i < NUM_CHALES; i++) {
        if (g_chales[i].status[0] == LIVRE) {
            livres++;
        }
    }
    return livres;
}

void atualizar_status_chale(const char *id_chale, int dia_offset, StatusChale novo_status) {
    inicializar_chales();
    for (int i = 0; i < NUM_CHALES; i++) {
        if (strcmp(g_chales[i].id, id_chale) == 0) {
            if (dia_offset >= 0 && dia_offset < TOTAL_DIAS) {
                g_chales[i].status[dia_offset] = novo_status;
            }
            break;
        }
    }
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

EstadoSistema tela_mapa_ocupacao(void) {
    inicializar_chales();

    time_t data_inicial = time(NULL);
    int offset_dias = 0;
    int opcao;

    while (1) {
        LIMPAR_TELA();

        imprimir_cabecalho(data_inicial);
        for (int i = 0; i < NUM_CHALES; i++) {
            imprimir_linha_chale(&g_chales[i], offset_dias);
        }
        imprimir_legenda();

        printf("[1] Avancar 7 dias   [2] Voltar 7 dias   [0] Voltar ao Menu Principal\n");
        printf("Opcao > ");

        opcao = ler_opcao_segura();

        switch (opcao) {
            case 1:
                if (offset_dias + 7 < TOTAL_DIAS) {
                    offset_dias += 7;
                    data_inicial += 7 * 24 * 60 * 60;
                } else {
                    printf("\nLimite máximo de exibição de dias alcançado.\n");
                    pausar_sistema();
                }
                break;
            case 2:
                if (offset_dias >= 7) {
                    offset_dias -= 7;
                    data_inicial -= 7 * 24 * 60 * 60;
                } else {
                    printf("\nVocê já está visualizando a data inicial.\n");
                    pausar_sistema();
                }
                break;
            case 0:
                return ESTADO_MENU_PRINCIPAL;
            default:
                printf("\nOpcao invalida! Digite 1, 2 ou 0.\n");
                pausar_sistema();
                break;
        }
    }
}
