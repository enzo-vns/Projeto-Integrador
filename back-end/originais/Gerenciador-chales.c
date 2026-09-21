#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
 
#define NUM_CHALES     4
#define DIAS_VISIVEIS  5   
#define TOTAL_DIAS     14  
 

typedef enum {
    ESTADO_MENU_PRINCIPAL,
    ESTADO_MAPA_OCUPACAO,
    ESTADO_SAIR
} EstadoSistema;
 
typedef enum {
    LIVRE,
    RESERVADO,
    OCUPADO,
    LIMPEZA,
    MANUTENCAO
} StatusChale;
 
typedef struct {
    char id[8];
    char tipo[12];
    StatusChale status[TOTAL_DIAS];
} Chale;
 

static const char *rotulo_status(StatusChale s) {
    switch (s) {
        case LIVRE:      return "[ LIVR ]";
        case RESERVADO:  return "[ RESV ]";
        case OCUPADO:    return "[ OCUP ]";
        case LIMPEZA:    return "[ LIMP ]";
        case MANUTENCAO: return "[ MANU ]";
        default:         return "[ ???? ]";
    }
}
 
static const char *dia_semana_abrev(int wday) {
    static const char *dias[] = { "Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab" };
    return dias[wday % 7];
}
 

static void carregar_dados_exemplo(Chale chales[NUM_CHALES]) {
    StatusChale s0[TOTAL_DIAS] = { OCUPADO, OCUPADO, LIVRE, LIVRE, RESERVADO, LIVRE, LIVRE,
                                    LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
    StatusChale s1[TOTAL_DIAS] = { LIMPEZA, LIVRE, RESERVADO, RESERVADO, RESERVADO, LIVRE, LIVRE,
                                    LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
    StatusChale s2[TOTAL_DIAS] = { LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE,
                                    LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
    StatusChale s3[TOTAL_DIAS] = { MANUTENCAO, MANUTENCAO, MANUTENCAO, LIVRE, LIVRE, LIVRE, LIVRE,
                                    LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
 
    strcpy(chales[0].id, "CH-01"); strcpy(chales[0].tipo, "Luxo");
    memcpy(chales[0].status, s0, sizeof(s0));
 
    strcpy(chales[1].id, "CH-02"); strcpy(chales[1].tipo, "Familia");
    memcpy(chales[1].status, s1, sizeof(s1));
 
    strcpy(chales[2].id, "CH-03"); strcpy(chales[2].tipo, "Standard");
    memcpy(chales[2].status, s2, sizeof(s2));
 
    strcpy(chales[3].id, "CH-04"); strcpy(chales[3].tipo, "Standard");
    memcpy(chales[3].status, s3, sizeof(s3));
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
    Chale chales[NUM_CHALES];
    carregar_dados_exemplo(chales);
 
    time_t data_inicial = time(NULL);
    int offset_dias = 0;
    int opcao;
 
    while (1) {
        system("clear");
 
        imprimir_cabecalho(data_inicial);
        for (int i = 0; i < NUM_CHALES; i++) {
            imprimir_linha_chale(&chales[i], offset_dias);
        }
        imprimir_legenda();
 
        printf("[1] Avancar 7 dias   [2] Voltar 7 dias   [0] Voltar ao Menu\n");
        printf("Opcao > ");
 
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            continue;
        }
 
        switch (opcao) {
            case 1:
                offset_dias += 7;
                data_inicial += 7 * 24 * 60 * 60;
                break;
            case 2:
                if (offset_dias >= 7) {
                    offset_dias -= 7;
                    data_inicial -= 7 * 24 * 60 * 60;
                }
                break;
            case 0:
                return ESTADO_MENU_PRINCIPAL;
            default:
                printf("Opcao invalida! Pressione ENTER para continuar...");
                getchar();
                getchar();
        }
    }
}
 
int main(void) {
    EstadoSistema estado = ESTADO_MAPA_OCUPACAO;
 
    while (estado != ESTADO_SAIR) {
        switch (estado) {
            case ESTADO_MAPA_OCUPACAO:
                estado = tela_mapa_ocupacao();
                break;
            case ESTADO_MENU_PRINCIPAL:
                printf("\n(Aqui entraria o menu principal - encerrando o teste)\n");
                estado = ESTADO_SAIR;
                break;
            default:
                estado = ESTADO_SAIR;
        }
    }
 
    return 0;
}
 