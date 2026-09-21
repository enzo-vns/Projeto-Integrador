#include "common.h"
#include "auth.h"
#include "recepcao.h"
#include "chales.h"
#include "auditoria.h"

int main(void) {
    // Inicializa os subsistemas
    inicializar_chales();
    inicializar_auditoria();
    inicializar_dados_hotel();

    SessaoUsuario sessao;

    while (1) {
        memset(&sessao, 0, sizeof(sessao));

        // Tela de login inicial (desenvolvida por Rayane)
        int logado = tela_login(&sessao);
        if (!logado) {
            break; // Usuário escolheu sair
        }

        // Direcionamento com base no perfil de acesso autenticado
        switch (sessao.perfil) {
            case PERFIL_RECEPCAO:
            case PERFIL_ADMIN:
                // Painel de Recepção completo (desenvolvido por João, integrado a Carlos e Rayane)
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
                    printf(" [2] Visualizar Mapa de Ocupação da Semana\n");
                    printf(" [0] Fazer Logoff\n\n");
                    printf("Opção > ");
                    op = ler_opcao_segura();

                    if (op == 1) {
                        processar_governanca(&sessao);
                    } else if (op == 2) {
                        tela_mapa_ocupacao();
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
    printf(" Recanto do Bosque - Sistema Integrado Encerrado. Até mais!\n");
    printf("================================================================================\n");

    return 0;
}
