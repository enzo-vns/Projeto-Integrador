#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_CHALES 4


typedef enum {
    ESTADO_MENU_PRINCIPAL,
    ESTADO_GOVERNANCA,
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
    StatusChale status;
} Chale;


static const char *texto_status(StatusChale s) {
    switch (s) {
        case LIVRE:      return "Disponivel";
        case RESERVADO:  return "Reservado";
        case OCUPADO:    return "Ocupado";
        case LIMPEZA:    return "Limpeza";
        case MANUTENCAO: return "Manutencao";
        default:         return "Desconhecido";
    }
}


static const char *texto_acao(StatusChale s) {
    switch (s) {
        case OCUPADO:    return "[ Bloqueado ]";
        case LIMPEZA:    return "[1] Liberar para DISPONIVEL";
        case LIVRE:      return "[2] Colocar em MANUTENCAO";
        case MANUTENCAO: return "[3] Concluir Manutencao (LIBERAR)";
        default:         return "[ Sem acao ]";
    }
}


static void carregar_dados_exemplo(Chale chales[NUM_CHALES]) {
    strcpy(chales[0].id, "CH-01"); strcpy(chales[0].tipo, "Luxo");     chales[0].status = OCUPADO;
    strcpy(chales[1].id, "CH-02"); strcpy(chales[1].tipo, "Familia");  chales[1].status = LIMPEZA;
    strcpy(chales[2].id, "CH-03"); strcpy(chales[2].tipo, "Standard"); chales[2].status = LIVRE;
    strcpy(chales[3].id, "CH-04"); strcpy(chales[3].tipo, "Standard"); chales[3].status = MANUTENCAO;
}

static void imprimir_tabela(Chale chales[NUM_CHALES]) {
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | PAINEL DA GOVERNANCA E MANUTENCAO                            \n");
    printf("================================================================================\n\n");

    printf("%-4s | %-8s | %-8s | %-12s | %s\n", "ID", "CHALE", "TIPO", "STATUS ATUAL", "ACAO DISPONIVEL");
    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < NUM_CHALES; i++) {
        printf("%02d   | %-8s | %-8s | %-12s | %s\n",
               i + 1,
               chales[i].id,
               chales[i].tipo,
               texto_status(chales[i].status),
               texto_acao(chales[i].status));
    }
    printf("--------------------------------------------------------------------------------\n\n");
}


static int aplicar_acao(Chale *c) {
    switch (c->status) {
        case LIMPEZA:
            c->status = LIVRE;
            printf("\n%s liberado! Novo status: %s\n", c->id, texto_status(c->status));
            return 1;
        case LIVRE:
            c->status = MANUTENCAO;
            printf("\n%s colocado em manutencao! Novo status: %s\n", c->id, texto_status(c->status));
            return 1;
        case MANUTENCAO:
            c->status = LIVRE;
            printf("\n%s liberado apos manutencao! Novo status: %s\n", c->id, texto_status(c->status));
            return 1;
        case OCUPADO:
        default:
            printf("\nEste chale esta BLOQUEADO no momento, nao ha acao disponivel.\n");
            return 0;
    }
}


EstadoSistema tela_governanca(void) {
    Chale chales[NUM_CHALES];
    carregar_dados_exemplo(chales);

    int escolha;

    while (1) {
        system("clear"); /* no Windows, use "cls" */

        imprimir_tabela(chales);
        printf("Digite o ID do chale para aplicar a acao mostrada (ou 0 para Sair) > ");

        if (scanf("%d", &escolha) != 1) {
            while (getchar() != '\n');
            continue;
        }

        if (escolha == 0) {
            return ESTADO_MENU_PRINCIPAL;
        }

        if (escolha < 1 || escolha > NUM_CHALES) {
            printf("\nID invalido! Pressione ENTER para continuar...");
            getchar();
            getchar();
            continue;
        }

        aplicar_acao(&chales[escolha - 1]);
        printf("Pressione ENTER para continuar...");
        getchar();
        getchar();
    }
}


int main(void) {
    EstadoSistema estado = ESTADO_GOVERNANCA;

    while (estado != ESTADO_SAIR) {
        switch (estado) {
            case ESTADO_GOVERNANCA:
                estado = tela_governanca();
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