#ifndef TELAS_H
#define TELAS_H

#include "../backend/backend.h"

/* LOGIN */
// Exibe a tela de login inicial no terminal (desenvolvida originalmente por Rayane)
// Retorna 1 se o login teve sucesso, ou 0 se o usuário optou por sair
int exibir_tela_login(SessaoUsuario *sessao);

/* MAPA */
// Exibe o mapa de ocupação semanal dos chalés (desenvolvido originalmente por Carlos)
EstadoSistema exibir_tela_mapa(const SessaoUsuario *sessao);

/* AUDITORIA */
// Exibe o relatório interativo de auditoria (desenvolvido originalmente por Rayane)
EstadoSistema exibir_tela_auditoria(const SessaoUsuario *sessao);

/* OPERACOES */
// Subtelas do sistema hoteleiro (Front-end de console)
EstadoSistema exibir_tela_disponibilidade(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_checkin(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_consumo(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_checkout(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_alterar_cancelar(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_hospedes(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_governanca(const SessaoUsuario *sessao);

EstadoSistema exibir_tela_chales(const SessaoUsuario *sessao);
EstadoSistema exibir_tela_relatorio(const SessaoUsuario *sessao);

/* RECEPCAO */
// Executa o loop principal do painel da recepção (desenvolvido originalmente por João)
void executar_painel_recepcao(const SessaoUsuario *sessao);

#endif
