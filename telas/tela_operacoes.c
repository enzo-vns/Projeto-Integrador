#include "tela_operacoes.h"
#include "../backend/chales.h"
#include "../backend/reservas.h"
#include "../backend/hospedes.h"
#include "../backend/consumo.h"
#include "../backend/auditoria.h"

EstadoSistema exibir_tela_disponibilidade(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | DISPONIBILIDADE & NOVA RESERVA\n");
    printf("================================================================================\n\n");

    printf("STATUS DOS CHALES HOJE:\n");
    Chale *chales = obter_chales();
    int total_chales = obter_total_chales();
    for (int i = 0; i < total_chales; i++) {
        printf(" - [%s] Tipo: %-10s | Status: %s\n",
               chales[i].id, chales[i].tipo, nome_status(chales[i].status[0]));
    }

    printf("\nRESERVAS EXISTENTES:\n");
    printf("%-5s | %-18s | %-8s | %-12s | %-12s | %s\n",
           "ID", "Hospede", "Chale", "Check-in", "Check-out", "Status");
    printf("--------------------------------------------------------------------------------\n");

    Reserva *reservas = obter_reservas();
    int total_reservas = obter_total_reservas();
    for (int i = 0; i < total_reservas; i++) {
        printf("#%-4d | %-18s | %-8s | %-12s | %-12s | %s\n",
               reservas[i].id,
               reservas[i].hospede,
               reservas[i].chale,
               reservas[i].checkin,
               reservas[i].checkout,
               reservas[i].status);
    }

    printf("\n[1] Criar Nova Reserva    [0] Voltar ao Menu\nOpcao > ");
    int op = ler_opcao_segura();

    if (op == 1) {
        char nome[50], chale[8], checkin[12], checkout[12];

        printf("\nNome do Hospede: ");
        if (scanf("%49[^\n]", nome) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        printf("ID do Chale (ex: CH-03): ");
        if (scanf("%7s", chale) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        printf("Data Check-in (DD/MM/AAAA): ");
        if (scanf("%11s", checkin) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        printf("Data Check-out (DD/MM/AAAA): ");
        if (scanf("%11s", checkout) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        int novo_id = criar_reserva(nome, chale, checkin, checkout);
        if (novo_id > 0) {
            char detalhe[80];
            snprintf(detalhe, sizeof(detalhe), "Reserva #%d criada (%s, %s)", novo_id, nome, chale);
            adicionar_log_auditoria(sessao->usuario, "NOVA_RESERVA", detalhe);
            printf("\n[SUCESSO] Reserva #%d cadastrada com sucesso!\n", novo_id);
        } else {
            printf("\n[ERRO] Nao foi possivel cadastrar a reserva (limite atingido).\n");
        }
        pausar_sistema();
    }

    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema exibir_tela_checkin(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | REALIZAR CHECK-IN\n");
    printf("================================================================================\n\n");

    printf("RESERVAS PENDENTES DE CHECK-IN:\n");
    printf("%-5s | %-18s | %-8s | %-12s | %s\n", "ID", "Hospede", "Chale", "Check-in", "Status");
    printf("--------------------------------------------------------------------------------\n");

    Reserva *reservas = obter_reservas();
    int total_reservas = obter_total_reservas();
    int pendentes = 0;

    for (int i = 0; i < total_reservas; i++) {
        if (strcmp(reservas[i].status, "confirmada") == 0 && !reservas[i].checkedIn) {
            printf("#%-4d | %-18s | %-8s | %-12s | Confirmada\n",
                   reservas[i].id, reservas[i].hospede, reservas[i].chale, reservas[i].checkin);
            pendentes++;
        }
    }

    if (pendentes == 0) {
        printf("Nenhum check-in pendente no momento.\n");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }

    printf("\nDigite o ID da reserva para realizar o check-in (ou 0 para voltar): ");
    int id_reserva = ler_opcao_segura();
    if (id_reserva == 0) return ESTADO_MENU_PRINCIPAL;

    Reserva *r = buscar_reserva_por_id(id_reserva);
    if (r && !r->checkedIn) {
        efetuar_checkin(id_reserva);

        char detalhe[80];
        snprintf(detalhe, sizeof(detalhe), "Reserva #%d (Hospede: %s)", r->id, r->hospede);
        adicionar_log_auditoria(sessao->usuario, "CHECKIN_REALIZADO", detalhe);

        printf("\n[SUCESSO] Check-in da Reserva #%d realizado! Chale %s alterado para OCUPADO.\n",
               r->id, r->chale);
    } else {
        printf("\nReserva nao encontrada ou ja com check-in efetuado.\n");
    }

    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema exibir_tela_consumo(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | LANCAR CONSUMO / EXTRATO DA CONTA\n");
    printf("================================================================================\n\n");

    printf("HOSPEDES ATUALMENTE HOSPEDADOS (COM CHECK-IN ATIVO):\n");
    Reserva *reservas = obter_reservas();
    int total_reservas = obter_total_reservas();
    int ativos = 0;

    for (int i = 0; i < total_reservas; i++) {
        if (reservas[i].checkedIn && !reservas[i].checkedOut) {
            printf(" - [%s] Reserva #%d: %s\n", reservas[i].chale, reservas[i].id, reservas[i].hospede);
            ativos++;
        }
    }

    if (ativos == 0) {
        printf("Nenhum hospede com check-in ativo no momento.\n");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }

    char chale[8];
    char item[50];
    float valor;

    printf("\nDigite o ID do Chale (ex: CH-01): ");
    if (scanf("%7s", chale) != 1) {
        limpar_buffer();
        return ESTADO_MENU_PRINCIPAL;
    }
    limpar_buffer();

    printf("Descricao do consumo (ex: Refrigerante e Lanche): ");
    if (scanf("%49[^\n]", item) != 1) {
        limpar_buffer();
        return ESTADO_MENU_PRINCIPAL;
    }
    limpar_buffer();

    printf("Valor em R$ (ex: 28.50): ");
    if (scanf("%f", &valor) != 1) {
        limpar_buffer();
        return ESTADO_MENU_PRINCIPAL;
    }
    limpar_buffer();

    lancar_consumo(chale, item, valor);

    char detalhe[80];
    snprintf(detalhe, sizeof(detalhe), "%s: %s (R$ %.2f)", chale, item, valor);
    adicionar_log_auditoria(sessao->usuario, "LANCAMENTO_CONSUMO", detalhe);

    printf("\n[SUCESSO] Consumo de R$ %.2f lancado com sucesso na conta do %s!\n", valor, chale);
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema exibir_tela_checkout(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | REALIZAR CHECK-OUT & FECHAMENTO\n");
    printf("================================================================================\n\n");

    printf("HOSPEDES HOSPEDADOS (PENDENTES DE CHECK-OUT):\n");
    printf("%-5s | %-18s | %-8s | %-12s\n", "ID", "Hospede", "Chale", "Check-out Previsto");
    printf("--------------------------------------------------------------------------------\n");

    Reserva *reservas = obter_reservas();
    int total_reservas = obter_total_reservas();
    int ativos = 0;

    for (int i = 0; i < total_reservas; i++) {
        if (reservas[i].checkedIn && !reservas[i].checkedOut) {
            printf("#%-4d | %-18s | %-8s | %-12s\n",
                   reservas[i].id, reservas[i].hospede, reservas[i].chale, reservas[i].checkout);
            ativos++;
        }
    }

    if (ativos == 0) {
        printf("Nenhum hospede ativo para check-out no momento.\n");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }

    printf("\nDigite o ID da reserva para realizar check-out (ou 0 para voltar): ");
    int id_reserva = ler_opcao_segura();
    if (id_reserva == 0) return ESTADO_MENU_PRINCIPAL;

    Reserva *r = buscar_reserva_por_id(id_reserva);
    if (r && r->checkedIn && !r->checkedOut) {
        efetuar_checkout(id_reserva);

        char detalhe[80];
        snprintf(detalhe, sizeof(detalhe), "Reserva #%d finalizada. %s enviado para LIMPEZA",
                 r->id, r->chale);
        adicionar_log_auditoria(sessao->usuario, "CHECKOUT_REALIZADO", detalhe);

        printf("\n[SUCESSO] Check-out concluido! Chale %s alterado para LIMPEZA.\n", r->chale);
    } else {
        printf("\nReserva nao encontrada ou ja finalizada.\n");
    }

    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema exibir_tela_alterar_cancelar(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | ALTERAR / CANCELAR RESERVA\n");
    printf("================================================================================\n\n");

    Reserva *reservas = obter_reservas();
    int total_reservas = obter_total_reservas();

    printf("%-5s | %-18s | %-8s | %-12s | %s\n", "ID", "Hospede", "Chale", "Check-in", "Status");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < total_reservas; i++) {
        printf("#%-4d | %-18s | %-8s | %-12s | %s\n",
               reservas[i].id, reservas[i].hospede, reservas[i].chale, reservas[i].checkin, reservas[i].status);
    }

    printf("\nDigite o ID da reserva para cancelar (ou 0 para voltar): ");
    int id_reserva = ler_opcao_segura();
    if (id_reserva == 0) return ESTADO_MENU_PRINCIPAL;

    int resultado = cancelar_reserva(id_reserva);
    if (resultado == 1) {
        char motivo[60];
        printf("Motivo do cancelamento: ");
        if (scanf("%59[^\n]", motivo) != 1) {
            strcpy(motivo, "Desistencia");
        }
        limpar_buffer();

        char detalhe[80];
        snprintf(detalhe, sizeof(detalhe), "Res #%d - Motivo: %s", id_reserva, motivo);
        adicionar_log_auditoria(sessao->usuario, "CANCELA_RESERVA", detalhe);

        printf("\n[SUCESSO] Reserva #%d cancelada com sucesso!\n", id_reserva);
    } else if (resultado == -1) {
        printf("\nEsta reserva ja se encontra cancelada.\n");
    } else {
        printf("\nReserva nao encontrada.\n");
    }

    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema exibir_tela_hospedes(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | BUSCAR / CADASTRAR HOSPEDE\n");
    printf("================================================================================\n\n");

    Hospede *hospedes = obter_hospedes();
    int total_hospedes = obter_total_hospedes();

    printf("%-4s | %-20s | %-16s | %s\n", "ID", "Nome", "Documento/CPF", "Telefone");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < total_hospedes; i++) {
        printf("#%-3d | %-20s | %-16s | %s\n",
               hospedes[i].id, hospedes[i].nome, hospedes[i].documento, hospedes[i].telefone);
    }

    printf("\n[1] Cadastrar Novo Hospede    [0] Voltar ao Menu\nOpcao > ");
    int op = ler_opcao_segura();

    if (op == 1) {
        char nome[50], doc[20], tel[20];

        printf("\nNome Completo: ");
        if (scanf("%49[^\n]", nome) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        printf("CPF/Documento: ");
        if (scanf("%19[^\n]", doc) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        printf("Telefone: ");
        if (scanf("%19[^\n]", tel) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        int novo_id = cadastrar_hospede(nome, doc, tel);
        if (novo_id > 0) {
            char detalhe[80];
            snprintf(detalhe, sizeof(detalhe), "Hospede cadastrado: %s (CPF: %s)", nome, doc);
            adicionar_log_auditoria(sessao->usuario, "CADASTRO_HOSPEDE", detalhe);
            printf("\n[SUCESSO] Hospede %s cadastrado com ID #%d!\n", nome, novo_id);
        } else {
            printf("\n[ERRO] Limite maximo de hospedes atingido.\n");
        }
        pausar_sistema();
    }

    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema exibir_tela_governanca(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | GOVERNANCA & MANUTENCAO DE CHALES\n");
    printf("================================================================================\n\n");

    Chale *chales = obter_chales();
    int total_chales = obter_total_chales();
    for (int i = 0; i < total_chales; i++) {
        printf(" [%s] Tipo: %-10s | Status Atual: %s\n",
               chales[i].id, chales[i].tipo, nome_status(chales[i].status[0]));
    }

    printf("\n--------------------------------------------------------------------------------\n");
    printf("Digite o ID do chale para atualizar status (ex: CH-02 ou 0 para voltar): ");

    char id_chale[8];
    if (scanf("%7s", id_chale) != 1) {
        limpar_buffer();
        return ESTADO_MENU_PRINCIPAL;
    }
    limpar_buffer();

    if (strcmp(id_chale, "0") == 0) return ESTADO_MENU_PRINCIPAL;

    printf("\nNovo status para %s:\n", id_chale);
    printf(" [1] LIVRE (Limpeza concluida / Pronto)\n");
    printf(" [2] LIMPEZA\n");
    printf(" [3] MANUTENCAO\n");
    printf(" [4] OCUPADO\n");
    printf("Opcao > ");

    int st = ler_opcao_segura();
    StatusChale novo_st;
    switch (st) {
        case 1: novo_st = LIVRE; break;
        case 2: novo_st = LIMPEZA; break;
        case 3: novo_st = MANUTENCAO; break;
        case 4: novo_st = OCUPADO; break;
        default:
            printf("\nOpcao invalida.\n");
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
    }

    atualizar_status_chale(id_chale, 0, novo_st);

    char detalhe[80];
    snprintf(detalhe, sizeof(detalhe), "%s alterado para %s", id_chale, nome_status(novo_st));
    adicionar_log_auditoria(sessao->usuario, "STATUS_CHALE", detalhe);

    printf("\n[SUCESSO] Status do %s alterado para %s!\n", id_chale, nome_status(novo_st));
    pausar_sistema();

    return ESTADO_MENU_PRINCIPAL;
}
