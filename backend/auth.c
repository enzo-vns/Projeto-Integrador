#include "auth.h"
#include "auditoria.h"

int autenticar_usuario(const char *usuario, const char *senha, SessaoUsuario *sessao) {
    if (!usuario || !senha || !sessao) return 0;

    if (strcmp(usuario, "marcos.recepcao") == 0 && strcmp(senha, "123456") == 0) {
        strcpy(sessao->usuario, "marcos.rec");
        strcpy(sessao->nome, "Marcos (Recepcao)");
        sessao->perfil = PERFIL_RECEPCAO;
        adicionar_log_auditoria(sessao->usuario, "LOGIN", "Acesso ao painel da recepcao");
        return 1;
    }

    if (strcmp(usuario, "diretoria.admin") == 0 && (strcmp(senha, "123456") == 0 || strcmp(senha, "admin") == 0)) {
        strcpy(sessao->usuario, "diretoria");
        strcpy(sessao->nome, "Diretoria (Administrador)");
        sessao->perfil = PERFIL_ADMIN;
        adicionar_log_auditoria(sessao->usuario, "LOGIN", "Acesso ao painel administrativo");
        return 1;
    }

    if (strcmp(usuario, "ana.governanca") == 0 && strcmp(senha, "123456") == 0) {
        strcpy(sessao->usuario, "ana.gov");
        strcpy(sessao->nome, "Ana (Governanca)");
        sessao->perfil = PERFIL_GOVERNANCA;
        adicionar_log_auditoria(sessao->usuario, "LOGIN", "Acesso ao painel de governanca");
        return 1;
    }

    return 0;
}
