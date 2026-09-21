#ifndef TELA_LOGIN_H
#define TELA_LOGIN_H

#include "../common/common.h"

// Exibe a tela de login inicial no terminal (desenvolvida originalmente por Rayane)
// Retorna 1 se o login teve sucesso, ou 0 se o usuário optou por sair
int exibir_tela_login(SessaoUsuario *sessao);

#endif // TELA_LOGIN_H
