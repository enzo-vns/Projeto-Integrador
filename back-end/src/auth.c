#include "auth.h"
#include "auditoria.h"

int tela_login(SessaoUsuario *sessao) {
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

        // Verificação dos perfis do sistema
        if (strcmp(usuario, "marcos.recepcao") == 0 && strcmp(senha, "123456") == 0) {
            strcpy(sessao->usuario, "marcos.rec");
            strcpy(sessao->nome, "Marcos (Recepcao)");
            sessao->perfil = PERFIL_RECEPCAO;

            printf("\nLogin realizado com sucesso!\n");
            printf("================================================================================\n");
            printf("ENCAMINHANDO PARA PAINEL DE RECEPCAO...\n");
            printf("================================================================================\n");
            adicionar_log_auditoria(sessao->usuario, "LOGIN", "Acesso ao painel da recepcao");
            pausar_sistema();
            return 1;
        } else if (strcmp(usuario, "diretoria.admin") == 0 && (strcmp(senha, "123456") == 0 || strcmp(senha, "admin") == 0)) {
            strcpy(sessao->usuario, "diretoria");
            strcpy(sessao->nome, "Diretoria (Administrador)");
            sessao->perfil = PERFIL_ADMIN;

            printf("\nLogin de Administrador realizado com sucesso!\n");
            printf("================================================================================\n");
            printf("ENCAMINHANDO PARA PAINEL ADMINISTRATIVO...\n");
            printf("================================================================================\n");
            adicionar_log_auditoria(sessao->usuario, "LOGIN", "Acesso ao painel administrativo");
            pausar_sistema();
            return 1;
        } else if (strcmp(usuario, "ana.governanca") == 0 && strcmp(senha, "123456") == 0) {
            strcpy(sessao->usuario, "ana.gov");
            strcpy(sessao->nome, "Ana (Governanca)");
            sessao->perfil = PERFIL_GOVERNANCA;

            printf("\nLogin de Governanca realizado com sucesso!\n");
            printf("================================================================================\n");
            printf("ENCAMINHANDO PARA PAINEL DE GOVERNANCA...\n");
            printf("================================================================================\n");
            adicionar_log_auditoria(sessao->usuario, "LOGIN", "Acesso ao modulo de governanca");
            pausar_sistema();
            return 1;
        } else {
            printf("\n[ERRO] Usuario ou senha incorretos!\n");
            printf("Dica de teste: 'marcos.recepcao' / '123456' ou 'diretoria.admin' / '123456'\n");
            pausar_sistema();
        }
    }
}
