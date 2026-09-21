#include <stdio.h>
#include <stdlib.h>

// Define o comando de limpar tela dependendo se é Windows ou Linux/Mac
#ifdef _WIN32
 #define LIMPAR_TELA() system("cls")
#else
 #define LIMPAR_TELA() system("clear")
#endif

// Estados das telas do sistema (máquina de estados)
typedef enum {
    ESTADO_SAIR = 0,
    ESTADO_MENU_PRINCIPAL,
    ESTADO_DISPONIBILIDADE,
    ESTADO_MAPA_OCUPACAO,
    ESTADO_CHECK_IN,
    ESTADO_CONSUMO,
    ESTADO_CHECK_OUT,
    ESTADO_ALTERAR_CANCELAR,
    ESTADO_HOSPEDE
} EstadoSistema;

// Protótipos das funções
void limpar_buffer();
int ler_opcao_segura();
void pausar_sistema();

EstadoSistema processar_menu_principal();
EstadoSistema processar_disponibilidade();
EstadoSistema processar_mapa_ocupacao();
EstadoSistema processar_check_in();
EstadoSistema processar_consumo();
EstadoSistema processar_check_out();
EstadoSistema processar_alterar_cancelar();
EstadoSistema processar_hospede();

// Função principal: controla o loop do menu baseado no estado atual
int main() {
    EstadoSistema estado_atual = ESTADO_MENU_PRINCIPAL;

    while (estado_atual != ESTADO_SAIR) {
        switch (estado_atual) {
            case ESTADO_MENU_PRINCIPAL:
                estado_atual = processar_menu_principal();
                break;
            case ESTADO_DISPONIBILIDADE:
                estado_atual = processar_disponibilidade();
                break;
            case ESTADO_MAPA_OCUPACAO:
                estado_atual = processar_mapa_ocupacao();
                break;
            case ESTADO_CHECK_IN:
                estado_atual = processar_check_in();
                break;
            case ESTADO_CONSUMO:
                estado_atual = processar_consumo();
                break;
            case ESTADO_CHECK_OUT:
                estado_atual = processar_check_out();
                break;
            case ESTADO_ALTERAR_CANCELAR:
                estado_atual = processar_alterar_cancelar();
                break;
            case ESTADO_HOSPEDE:
                estado_atual = processar_hospede();
                break;
            default:
                printf("\nErro: Estado não reconhecido. Voltando ao menu...\n");
                pausar_sistema();
                estado_atual = ESTADO_MENU_PRINCIPAL;
                break;
        }
    }

    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" Sistema encerrado. Até mais!\n");
    printf("================================================================================\n");
    return 0;
}

// Limpa o buffer do teclado pra evitar loop infinito se digitarem letras
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // descarta o lixo que sobrou
    }
}

// Lê a opção do usuário de forma segura contra erros de tipo
int ler_opcao_segura() {
    int opcao;
    int resultado = scanf("%d", &opcao);
    limpar_buffer();
    
    if (resultado != 1) {
        return -1; // Retorna -1 se digitarem alguma letra
    }
    return opcao;
}

// Pausa a tela pra o usuário conseguir ler a mensagem antes de limpar
void pausar_sistema() {
    printf("\nPressione [ENTER] para continuar...");
    getchar();
}

// Exibe a tela principal com o menu e o resumo do dia
EstadoSistema processar_menu_principal() {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | Operador: Marcos (Recepcao) | Data: 08/09/2026                \n");
    printf("================================================================================\n");
    printf("\nRESUMO DO DIA:\n");
    printf(" [ Check-ins Pendentes: 3 ]  [ Check-outs Pendentes: 2 ]  [ Chales Livres: 5 ]\n");
    printf("\n--------------------------------------------------------------------------------\n");
    printf("MENU PRINCIPAL:\n\n");
    printf(" [1] Consultar Disponibilidade & Nova Reserva\n");
    printf(" [2] Mapa de Ocupação (Visão Geral)\n");
    printf(" [3] Realizar Check-in\n");
    printf(" [4] Lançar Consumo / Extrato da Conta\n");
    printf(" [5] Realizar Check-out & Fechamento\n");
    printf(" [6] Alterar / Cancelar Reserva\n");
    printf(" [7] Buscar / Cadastrar Hóspede\n");
    printf(" [0] Sair (Logoff)\n");
    printf("\n--------------------------------------------------------------------------------\n");
    printf("Opção desejada > ");

    int opcao = ler_opcao_segura();

    // Direciona para o estado correspondente com base no número escolhido
    switch (opcao) {
        case 1: return ESTADO_DISPONIBILIDADE;
        case 2: return ESTADO_MAPA_OCUPACAO;
        case 3: return ESTADO_CHECK_IN;
        case 4: return ESTADO_CONSUMO;
        case 5: return ESTADO_CHECK_OUT;
        case 6: return ESTADO_ALTERAR_CANCELAR;
        case 7: return ESTADO_HOSPEDE;
        case 0: return ESTADO_SAIR;
        default:
            printf("\nOpção inválida! Digite apenas números do menu.\n");
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
    }
}

// Funções das sub-telas (mocks para preencher a navegação por enquanto)
EstadoSistema processar_disponibilidade() {
    LIMPAR_TELA();
    printf("=== MÓDULO: DISPONIBILIDADE E RESERVAS ===\n");
    printf("[Tela em construção]\n");
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_mapa_ocupacao() {
    LIMPAR_TELA();
    printf("=== MÓDULO: MAPA DE OCUPAÇÃO ===\n");
    printf("[Tela em construção]\n");
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_check_in() {
    LIMPAR_TELA();
    printf("=== MÓDULO: REALIZAR CHECK-IN ===\n");
    printf("[Tela em construção]\n");
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_consumo() {
    LIMPAR_TELA();
    printf("=== MÓDULO: LANÇAR CONSUMO / EXTRATO ===\n");
    printf("[Tela em construção]\n");
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_check_out() {
    LIMPAR_TELA();
    printf("=== MÓDULO: CHECK-OUT & FECHAMENTO ===\n");
    printf("[Tela em construção]\n");
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_alterar_cancelar() {
    LIMPAR_TELA();
    printf("=== MÓDULO: ALTERAR / CANCELAR RESERVA ===\n");
    printf("[Tela em construção]\n");
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_hospede() {
    LIMPAR_TELA();
    printf("=== MÓDULO: BUSCAR / CADASTRAR HÓSPEDE ===\n");
    printf("[Tela em construção]\n");
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}