#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "backend/backend.h"
#include "telas/telas.h"
int main(void) {
    inicializar_chales();
    inicializar_hospedes();
    inicializar_reservas();
    inicializar_consumo();
    SessaoUsuario sem_perfil = {0};
    SessaoUsuario recepcao = {.perfil = PERFIL_RECEPCAO};
    SessaoUsuario governanca = {.perfil = PERFIL_GOVERNANCA};
    assert(exibir_tela_auditoria(NULL) == ESTADO_MENU_PRINCIPAL);
    assert(exibir_tela_auditoria(&recepcao) == ESTADO_MENU_PRINCIPAL);
    assert(exibir_tela_chales(&recepcao) == ESTADO_MENU_PRINCIPAL);
    assert(exibir_tela_relatorio(&recepcao) == ESTADO_MENU_PRINCIPAL);
    assert(exibir_tela_checkin(&governanca) == ESTADO_MENU_PRINCIPAL);
    assert(exibir_tela_mapa(&sem_perfil) == ESTADO_MENU_PRINCIPAL);
    SessaoUsuario sessao = {0};
    assert(autenticar_usuario("marcos.recepcao", "123456", &sessao));
    assert(sessao.perfil == PERFIL_RECEPCAO);
    assert(!autenticar_usuario("diretoria.admin", "admin", &sessao));
    assert(autenticar_usuario("diretoria.admin", "123456", &sessao));
    assert(sessao.perfil == PERFIL_ADMIN);
    time_t agora = time(NULL);
    struct tm dia = *localtime(&agora);
    char hoje[12], amanha[12], ontem[12], datas[5][12];
    strftime(hoje, sizeof(hoje), "%d/%m/%Y", &dia);
    dia.tm_mday--;
    mktime(&dia);
    strftime(ontem, sizeof(ontem), "%d/%m/%Y", &dia);
    dia.tm_mday += 2;
    mktime(&dia);
    strftime(amanha, sizeof(amanha), "%d/%m/%Y", &dia);
    for (int i = 0; i < 5; i++) {
        dia.tm_mday++;
        mktime(&dia);
        strftime(datas[i], sizeof(datas[i]), "%d/%m/%Y", &dia);
    }
    assert(!periodo_reserva_valido(ontem, amanha));
    assert(periodo_reserva_valido(hoje, amanha));
    assert(criar_reserva(1, "CH-03", ontem, amanha) == -1);
    assert(data_numero("29/02/2024") > 0);
    assert(data_numero("29/02/2025") < 0);
    assert(!periodo_valido(datas[0], datas[0]));
    assert(criar_reserva(99, "CH-01", datas[0], datas[2]) == -1);
    assert(criar_reserva(1, "CH-99", datas[0], datas[2]) == -1);
    int a = criar_reserva(1, "CH-01", hoje, datas[2]);
    assert(a > 0);
    assert(criar_reserva(2, "CH-01", datas[1], datas[3]) == -2);
    int b = criar_reserva(2, "CH-01", datas[2], datas[4]);
    assert(b > 0);
    assert(alterar_reserva(b, 2, "CH-01", datas[1], datas[3]) == -2);
    assert(alterar_reserva(b, 2, "CH-01", ontem, amanha) == -1);
    assert(!efetuar_checkin(b)); /* reserva futura nao pode iniciar antes da data */
    assert(efetuar_checkin(a));
    assert(!efetuar_checkin(b));
    assert(lancar_consumo(b, "agua", 5) == -1);
    assert(lancar_consumo(a, "agua", 5) > 0);
    assert(total_consumo(a) == 5);
    assert(efetuar_checkout(a));
    assert(lancar_consumo(a, "agua", 5) == -1);
    assert(buscar_chale_por_id("CH-01")->status[0] == LIMPEZA);
    assert(cancelar_reserva(b) == 1);
    assert(cancelar_reserva(b) == -1);
    assert(cadastrar_chale("CH-05", "Familia", 4, 150));
    assert(!cadastrar_chale("CH-05", "Familia", 4, 150));
    assert(!cadastrar_chale("CH-06", "Familia", 4, NAN));
    assert(buscar_chale_por_id(NULL) == NULL);
    assert(cadastrar_hospede("Teste", "documento", NULL) == -1);

    int livres_antes = contar_chales_livres_hoje();
    assert(criar_reserva(1, "CH-02", hoje, amanha) > 0);
    assert(contar_chales_livres_hoje() == livres_antes - 1);

    for (int i = 0; i < MAX_LOGS + 1; i++) {
        adicionar_log_auditoria("usuario_com_nome_muito_longo_para_o_campo", "TESTE", "detalhe");
    }
    assert(obter_total_logs_auditoria() == MAX_LOGS);
    assert(strcmp(obter_logs_auditoria()[0].acao, "TESTE") == 0);
    assert(obter_logs_auditoria()[0].usuario[sizeof(obter_logs_auditoria()[0].usuario) - 1] ==
           '\0');
    assert(salvar_usuario("teste.login", "Pessoa Teste", PERFIL_RECEPCAO, "senha987"));
    assert(autenticar_usuario("teste.login", "senha987", &sessao));
    assert(!autenticar_usuario("teste.login", "senhaerrada", &sessao));
    assert(sessao.perfil == PERFIL_NENHUM);
    assert(salvar_usuario("teste.login", "Pessoa Teste", PERFIL_GOVERNANCA, "senha654"));
    assert(!autenticar_usuario("teste.login", "senha987", &sessao));
    assert(autenticar_usuario("teste.login", "senha654", &sessao));
    assert(!excluir_usuario("teste.login", "teste.login"));
    assert(!excluir_usuario("outro", "diretoria.admin"));
    assert(excluir_usuario("diretoria.admin", "teste.login"));
    assert(!autenticar_usuario("teste.login", "senha654", &sessao));
    assert(!excluir_chale("CH-01"));
    assert(!excluir_hospede(1));
    int hospede_sem_reserva = cadastrar_hospede("Novo", "doc-novo", "");
    assert(hospede_sem_reserva > 0);
    assert(excluir_hospede(hospede_sem_reserva));
    assert(!buscar_hospede_por_id(hospede_sem_reserva));
    assert(excluir_reserva(b));
    assert(!buscar_reserva_por_id(b));
    assert(excluir_chale("CH-05"));
    assert(!buscar_chale_por_id("CH-05"));
    puts("Testes de regras aprovados.");
    return 0;
}
