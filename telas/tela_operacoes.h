#ifndef TELA_OPERACOES_H
#define TELA_OPERACOES_H

#include "../backend/common.h"

// Subtelas do sistema hoteleiro (Front-end de console)
EstadoSistema exibir_tela_disponibilidade(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_checkin(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_consumo(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_checkout(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_alterar_cancelar(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_hospedes(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_governanca(const SessaoUsuario *sessao);

#endif // TELA_OPERACOES_H
