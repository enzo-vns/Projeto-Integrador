#include "tela_recepcao.h"
#include "tela_mapa.h"
#include "tela_auditoria.h"
#include "tela_operacoes.h"
#include "../backend/chales.h"
#include "../backend/reservas.h"

static EstadoSistema processar_menu_principal(const SessaoUsuario *sessao) {
    LIMPAR_TELA();

    char data_str[24];
    obter_data_hora_atual(data_str, sizeof(data_str));

    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | Operador: %-20s | Data: %s\n", sessao->nome, data_str);
    printf("================================================================================\n");
    printf("\nRESUMO DO DIA:\n");
    printf(" [ Check-ins Pendentes: %d ]  [ Check-outs Pendentes: %d ]  [ Chales Livres: %d ]\n",
           contar_checkins_pendentes(),
           contar_checkouts_pendentes(),
           contar_chales_livres_hoje());
    printf("\n--------------------------------------------------------------------------------\n");
    printf("MENU PRINCIPAL:\n\n");
    printf(" [1] Consultar Disponibilidade & Nova Reserva\n");
    printf(" [2] Mapa de Ocupação (Visão Geral - Carlos)\n");
    printf(" [3] Realizar Check-in\n");
    printf(" [4] Lançar Consumo / Extrato da Conta\n");
    printf(" [5] Realizar Check-out & Fechamento\n");
    printf(" [6] Alterar / Cancelar Reserva\n");
    printf(" [7] Buscar / Cadastrar Hóspede\n");
    printf(" [8] Relatório de Auditoria de Operações (Rayane)\n");
    printf(" [9] Governança / Manutenção de Chalés\n");
    printf(" [0] Sair (Logoff)\n");
    printf("\n--------------------------------------------------------------------------------\n");
    printf("Opção desejada > ");

    int opcao = ler_opcao_segura();

    switch (opcao) {
        case 1: return ESTADO_DISPONIBILIDADE;
        case 2: return ESTADO_MAPA_OCUPACAO;
        case 3: return ESTADO_CHECK_IN;
        case 4: return ESTADO_CONSUMO;
        case 5: return ESTADO_CHECK_OUT;
        case 6: return ESTADO_ALTERAR_CANCELAR;
        case 7: return ESTADO_HOSPEDE;
        case 8: return ESTADO_AUDITORIA;
        case 9: return ESTADO_GOVERNANCA;
        case 0: return ESTADO_SAIR;
        default:
            printf("\nOpção inválida! Digite apenas números do menu.\n");
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
    }
}

void executar_painel_recepcao(const SessaoUsuario *sessao) {
    EstadoSistema estado = ESTADO_MENU_PRINCIPAL;

    while (estado != ESTADO_SAIR) {
        switch (estado) {
            case ESTADO_MENU_PRINCIPAL:
                estado = processar_menu_principal(sessao);
                break;
            case ESTADO_DISPONIBILIDADE:
                estado = exibir_tela_disponibilidade(sessao);
                break;
            case ESTADO_MAPA_OCUPACAO:
                // Conexão com o mapa desenvolvido por Carlos
                estado = exibir_tela_mapa();
                break;
            case ESTADO_CHECK_IN:
                estado = exibir_tela_checkin(sessao);
                break;
            case ESTADO_CONSUMO:
                estado = exibir_tela_consumo(sessao);
                break;
            case ESTADO_CHECK_OUT:
                estado = exibir_tela_checkout(sessao);
                break;
            case ESTADO_ALTERAR_CANCELAR:
                estado = exibir_tela_alterar_cancelar(sessao);
                break;
            case ESTADO_HOSPEDE:
                estado = exibir_tela_hospedes(sessao);
                break;
            case ESTADO_AUDITORIA:
                // Conexão com o relatório de auditoria desenvolvido por Rayane
                estado = exibir_tela_auditoria();
                break;
            case ESTADO_GOVERNANCA:
                estado = exibir_tela_governanca(sessao);
                break;
            default:
                printf("\nEstado não reconhecido. Retornando ao menu principal...\n");
                pausar_sistema();
                estado = ESTADO_MENU_PRINCIPAL;
                break;
        }
    }
}
