#include "backend/common.h"
#include "backend/chales.h"
#include "backend/reservas.h"
#include "backend/hospedes.h"
#include "backend/consumo.h"
#include "backend/auditoria.h"
#include "telas/tela_login.h"
#include "telas/tela_recepcao.h"
#include "telas/tela_mapa.h"
#include "telas/tela_auditoria.h"
#include "telas/tela_operacoes.h"

int main(void) {
    // Inicialização dos dados e regras do Back-end
    inicializar_chales();
    inicializar_reservas();
    inicializar_hospedes();
    inicializar_consumo();
    inicializar_auditoria();

    SessaoUsuario sessao;

    while (1) {
        memset(&sessao, 0, sizeof(sessao));

        // Front-end: Tela de autenticação desenvolvida por Rayane
        int logado = exibir_tela_login(&sessao);
        if (!logado) {
            break; // Usuário optou por encerrar o sistema
        }

        // Direcionamento com base no perfil de acesso
        switch (sessao.perfil) {
            case PERFIL_RECEPCAO:
            case PERFIL_ADMIN:
                // Front-end: Painel da Recepção desenvolvido por João
                executar_painel_recepcao(&sessao);
                break;

            case PERFIL_GOVERNANCA: {
                int op = -1;
                while (op != 0) {
                    LIMPAR_TELA();
                    printf("================================================================================\n");
                    printf(" RECANTO BOSQUE | Painel da Governança | Operadora: %s\n", sessao.nome);
                    printf("================================================================================\n\n");
                    printf(" [1] Gerenciar Status e Limpeza dos Chalés\n");
                    printf(" [2] Visualizar Mapa de Ocupação da Semana (Carlos)\n");
                    printf(" [0] Fazer Logoff\n\n");
                    printf("Opção > ");
                    op = ler_opcao_segura();

                    if (op == 1) {
                        exibir_tela_governanca(&sessao);
                    } else if (op == 2) {
                        exibir_tela_mapa();
                    }
                }
                break;
            }

            default:
                printf("\nPerfil de usuário não reconhecido.\n");
                pausar_sistema();
                break;
        }
    }

    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" Recanto do Bosque - Sistema Integrado Encerrado com Sucesso. Até mais!\n");
    printf("================================================================================\n");

    return 0;
}
