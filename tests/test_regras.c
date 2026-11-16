#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "backend/backend.h"
int main(void) {
    inicializar_chales();
    inicializar_hospedes();
    inicializar_reservas();
    inicializar_consumo();
    assert(data_numero("29/02/2024") > 0);
    assert(data_numero("29/02/2025") < 0);
    assert(!periodo_valido("10/12/2026", "10/12/2026"));
    assert(criar_reserva(99, "CH-01", "10/12/2026", "12/12/2026") == -1);
    assert(criar_reserva(1, "CH-99", "10/12/2026", "12/12/2026") == -1);
    int a = criar_reserva(1, "CH-01", "10/12/2026", "12/12/2026");
    assert(a > 0);
    assert(criar_reserva(2, "CH-01", "11/12/2026", "13/12/2026") == -2);
    int b = criar_reserva(2, "CH-01", "12/12/2026", "14/12/2026");
    assert(b > 0);
    assert(alterar_reserva(b, 2, "CH-01", "11/12/2026", "13/12/2026") == -2);
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

    time_t agora = time(NULL);
    struct tm dia = *localtime(&agora);
    char hoje[12], amanha[12];
    strftime(hoje, sizeof(hoje), "%d/%m/%Y", &dia);
    dia.tm_mday++;
    mktime(&dia);
    strftime(amanha, sizeof(amanha), "%d/%m/%Y", &dia);
    int livres_antes = contar_chales_livres_hoje();
    assert(criar_reserva(1, "CH-02", hoje, amanha) > 0);
    assert(contar_chales_livres_hoje() == livres_antes - 1);

    for (int i = 0; i < MAX_LOGS + 1; i++) {
        adicionar_log_auditoria("usuario_com_nome_muito_longo_para_o_campo", "TESTE", "detalhe");
    }
    assert(obter_total_logs_auditoria() == MAX_LOGS);
    assert(strcmp(obter_logs_auditoria()[0].acao, "TESTE") == 0);
    assert(obter_logs_auditoria()[0].usuario[sizeof(obter_logs_auditoria()[0].usuario) - 1] == '\0');
    puts("Testes de regras aprovados.");
    return 0;
}
