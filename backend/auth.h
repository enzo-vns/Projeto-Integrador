#ifndef AUTH_H
#define AUTH_H

#include "common.h"

// Realiza a verificação de credenciais e retorna 1 se autenticado com sucesso
int autenticar_usuario(const char *usuario, const char *senha, SessaoUsuario *sessao);

#endif // AUTH_H
