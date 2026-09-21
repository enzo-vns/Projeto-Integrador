#ifndef AUTH_H
#define AUTH_H

#include "common.h"

// Realiza o fluxo de login do funcionário (baseado no código de Rayane)
// Retorna 1 se o login teve sucesso, 0 se o usuário optou por sair
int tela_login(SessaoUsuario *sessao);

#endif // AUTH_H
