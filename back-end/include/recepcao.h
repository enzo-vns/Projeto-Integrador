#ifndef RECEPCAO_H
#define RECEPCAO_H

#include "common.h"

// Inicializa dados de reservas e hóspedes
void inicializar_dados_hotel(void);

// Loop de navegação e menu principal (baseado no código de João)
EstadoSistema processar_menu_principal(const SessaoUsuario *sessao);

// Subtelas do painel de recepção
EstadoSistema processar_disponibilidade(const SessaoUsuario *sessao);
EstadoSistema processar_check_in(const SessaoUsuario *sessao);
EstadoSistema processar_consumo(const SessaoUsuario *sessao);
EstadoSistema processar_check_out(const SessaoUsuario *sessao);
EstadoSistema processar_alterar_cancelar(const SessaoUsuario *sessao);
EstadoSistema processar_hospede(const SessaoUsuario *sessao);
EstadoSistema processar_governanca(const SessaoUsuario *sessao);

// Executa o fluxo da recepção para a sessão atual
void executar_painel_recepcao(const SessaoUsuario *sessao);

#endif // RECEPCAO_H
