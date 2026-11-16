#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "backend/backend.h"
int main(void) {
    inicializar_chales(); inicializar_hospedes(); inicializar_reservas(); inicializar_consumo();
    assert(data_numero("29/02/2024") > 0);
    assert(data_numero("29/02/2025") < 0);
    assert(!periodo_valido("10/12/2026","10/12/2026"));
    assert(criar_reserva(99,"CH-01","10/12/2026","12/12/2026") == -1);
    assert(criar_reserva(1,"CH-99","10/12/2026","12/12/2026") == -1);
    int a=criar_reserva(1,"CH-01","10/12/2026","12/12/2026"); assert(a>0);
    assert(criar_reserva(2,"CH-01","11/12/2026","13/12/2026")==-2);
    int b=criar_reserva(2,"CH-01","12/12/2026","14/12/2026"); assert(b>0);
    assert(alterar_reserva(b,2,"CH-01","11/12/2026","13/12/2026")==-2);
    assert(efetuar_checkin(a)); assert(!efetuar_checkin(b));
    assert(lancar_consumo(b,"agua",5)==-1); assert(lancar_consumo(a,"agua",5)>0);
    assert(total_consumo(a)==5); assert(efetuar_checkout(a)); assert(lancar_consumo(a,"agua",5)==-1);
    assert(buscar_chale_por_id("CH-01")->status[0]==LIMPEZA);
    assert(cancelar_reserva(b)==1); assert(cancelar_reserva(b)==-1);
    assert(cadastrar_chale("CH-05","Familia",4,150)); assert(!cadastrar_chale("CH-05","Familia",4,150));
    puts("Testes de regras aprovados."); return 0;
}
