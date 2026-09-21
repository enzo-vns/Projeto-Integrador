#include "tela_login.h"
#include "../backend/auth.h"

int exibir_tela_login(SessaoUsuario *sessao) {
    char usuario[32];
    char senha[32];

    while (1) {
        LIMPAR_TELA();
        printf("================================================================================\n");
        printf("                      RECANTO DO BOSQUE | SISTEMA DE GESTAO                     \n");
        printf("================================================================================\n");
        printf("\nAutenticacao de funcionario (ou digite 'sair' para encerrar)\n\n");

        printf("Usuario: ");
        if (scanf("%31s", usuario) != 1) {
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        if (strcmp(usuario, "sair") == 0 || strcmp(usuario, "0") == 0) {
            return 0;
        }

        printf("Senha:   ");
        if (scanf("%31s", senha) != 1) {
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        if (autenticar_usuario(usuario, senha, sessao)) {
            printf("\nLogin realizado com sucesso!\n");
            printf("================================================================================\n");
            if (sessao->perfil == PERFIL_RECEPCAO) {
                printf("ENCAMINHANDO PARA PAINEL DE RECEPCAO...\n");
            } else if (sessao->perfil == PERFIL_ADMIN) {
                printf("ENCAMINHANDO PARA PAINEL ADMINISTRATIVO...\n");
            } else {
                printf("ENCAMINHANDO PARA PAINEL DE GOVERNANCA...\n");
            }
            printf("================================================================================\n");
            pausar_sistema();
            return 1;
        } else {
            printf("\n[ERRO] Usuario ou senha incorretos.\n");
            printf("Credenciais de teste:\n");
            printf(" - marcos.recepcao / 123456\n");
            printf(" - diretoria.admin / 123456\n");
            printf(" - ana.governanca  / 123456\n");
            pausar_sistema();
        }
    }
}
