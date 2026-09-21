#include "recepcao.h"
#include "chales.h"
#include "auditoria.h"

#define MAX_RESERVAS 50
#define MAX_HOSPEDES 50

typedef struct {
    int id;
    char hospede[50];
    char chale[8];
    char checkin[12];
    char checkout[12];
    char status[16];
    int checkedIn;
    int checkedOut;
} Reserva;

typedef struct {
    int id;
    char nome[50];
    char documento[20];
    char telefone[20];
} Hospede;

static Reserva g_reservas[MAX_RESERVAS];
static int g_total_reservas = 0;

static Hospede g_hospedes[MAX_HOSPEDES];
static int g_total_hospedes = 0;

static int g_dados_inicializados = 0;

void inicializar_dados_hotel(void) {
    if (g_dados_inicializados) return;

    inicializar_chales();
    inicializar_auditoria();

    // Hóspedes iniciais (alinhados com o front-end store.js)
    g_hospedes[0] = (Hospede){ 1, "Carlos Andrade", "321.654.987-00", "(11) 98888-1122" };
    g_hospedes[1] = (Hospede){ 2, "Beatriz Lima",   "112.334.556-70", "(11) 97777-3344" };
    g_hospedes[2] = (Hospede){ 3, "Renato Souza",   "998.877.665-40", "(11) 96666-5566" };
    g_total_hospedes = 3;

    // Reservas iniciais (alinhadas com front-end store.js)
    g_reservas[0] = (Reserva){ 104, "Carlos Andrade", "CH-01", "08/09/2026", "10/09/2026", "confirmada", 0, 0 };
    g_reservas[1] = (Reserva){ 105, "Beatriz Lima",   "CH-02", "10/09/2026", "13/09/2026", "confirmada", 0, 0 };
    g_reservas[2] = (Reserva){ 98,  "Renato Souza",   "CH-03", "05/09/2026", "07/09/2026", "cancelada",  0, 0 };
    g_total_reservas = 3;

    g_dados_inicializados = 1;
}

static int contar_checkins_pendentes(void) {
    int count = 0;
    for (int i = 0; i < g_total_reservas; i++) {
        if (strcmp(g_reservas[i].status, "confirmada") == 0 && !g_reservas[i].checkedIn) {
            count++;
        }
    }
    return count;
}

static int contar_checkouts_pendentes(void) {
    int count = 0;
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].checkedIn && !g_reservas[i].checkedOut) {
            count++;
        }
    }
    return count;
}

EstadoSistema processar_menu_principal(const SessaoUsuario *sessao) {
    inicializar_dados_hotel();
    LIMPAR_TELA();

    char data_str[24];
    obter_data_hora_atual(data_str, sizeof(data_str));

    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | Operador: %-20s | Data: %s\n", sessao->nome, data_str);
    printf("================================================================================\n");
    printf("\nRESUMO DO DIA:\n");
    printf(" [ Check-ins Pendentes: %d ]  [ Check-outs Pendentes: %d ]  [ Chales Livres: %d ]\n",
           contar_checkins_pendentes(),
           contar_checkouts_pendentes(),
           contar_chales_livres_hoje());
    printf("\n--------------------------------------------------------------------------------\n");
    printf("MENU PRINCIPAL:\n\n");
    printf(" [1] Consultar Disponibilidade & Nova Reserva\n");
    printf(" [2] Mapa de Ocupação (Visão Geral - Carlos)\n");
    printf(" [3] Realizar Check-in\n");
    printf(" [4] Lançar Consumo / Extrato da Conta\n");
    printf(" [5] Realizar Check-out & Fechamento\n");
    printf(" [6] Alterar / Cancelar Reserva\n");
    printf(" [7] Buscar / Cadastrar Hóspede\n");
    printf(" [8] Relatório de Auditoria de Operações (Rayane)\n");
    printf(" [9] Governança / Manutenção de Chalés\n");
    printf(" [0] Sair (Logoff)\n");
    printf("\n--------------------------------------------------------------------------------\n");
    printf("Opção desejada > ");

    int opcao = ler_opcao_segura();

    switch (opcao) {
        case 1: return ESTADO_DISPONIBILIDADE;
        case 2: return ESTADO_MAPA_OCUPACAO;
        case 3: return ESTADO_CHECK_IN;
        case 4: return ESTADO_CONSUMO;
        case 5: return ESTADO_CHECK_OUT;
        case 6: return ESTADO_ALTERAR_CANCELAR;
        case 7: return ESTADO_HOSPEDE;
        case 8: return ESTADO_AUDITORIA;
        case 9: return ESTADO_GOVERNANCA;
        case 0: return ESTADO_SAIR;
        default:
            printf("\nOpção inválida! Digite apenas números do menu.\n");
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
    }
}

EstadoSistema processar_disponibilidade(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | DISPONIBILIDADE & NOVA RESERVA\n");
    printf("================================================================================\n\n");

    printf("CHALES CADASTRADOS:\n");
    Chale *chales = obter_chales();
    for (int i = 0; i < NUM_CHALES; i++) {
        printf(" - [%s] %-10s | Status Hoje: %s\n",
               chales[i].id, chales[i].tipo, nome_status(chales[i].status[0]));
    }

    printf("\nRESERVAS EXISTENTES:\n");
    printf("%-5s | %-18s | %-8s | %-12s | %-12s | %s\n",
           "ID", "Hospede", "Chale", "Check-in", "Check-out", "Status");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < g_total_reservas; i++) {
        printf("#%-4d | %-18s | %-8s | %-12s | %-12s | %s\n",
               g_reservas[i].id,
               g_reservas[i].hospede,
               g_reservas[i].chale,
               g_reservas[i].checkin,
               g_reservas[i].checkout,
               g_reservas[i].status);
    }

    printf("\n[1] Criar Nova Reserva    [0] Voltar ao Menu\nOpcao > ");
    int op = ler_opcao_segura();
    if (op == 1 && g_total_reservas < MAX_RESERVAS) {
        Reserva nova;
        nova.id = 100 + g_total_reservas + 1;
        nova.checkedIn = 0;
        nova.checkedOut = 0;
        strcpy(nova.status, "confirmada");

        printf("\nNome do Hóspede: ");
        if (scanf("%49[^\n]", nova.hospede) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        printf("ID do Chalé (ex: CH-03): ");
        if (scanf("%7s", nova.chale) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        printf("Data Check-in (DD/MM/AAAA): ");
        if (scanf("%11s", nova.checkin) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        printf("Data Check-out (DD/MM/AAAA): ");
        if (scanf("%11s", nova.checkout) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        g_reservas[g_total_reservas++] = nova;
        atualizar_status_chale(nova.chale, 0, RESERVADO);

        char detalhe[80];
        snprintf(detalhe, sizeof(detalhe), "Reserva #%d criada (%s, %s)", nova.id, nova.hospede, nova.chale);
        adicionar_log_auditoria(sessao->usuario, "NOVA_RESERVA", detalhe);

        printf("\n[SUCESSO] Reserva #%d cadastrada com sucesso!\n", nova.id);
        pausar_sistema();
    }

    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_check_in(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | REALIZAR CHECK-IN\n");
    printf("================================================================================\n\n");

    printf("RESERVAS PENDENTES DE CHECK-IN:\n");
    printf("%-5s | %-18s | %-8s | %-12s | %s\n", "ID", "Hospede", "Chale", "Check-in", "Status");
    printf("--------------------------------------------------------------------------------\n");

    int pendentes = 0;
    for (int i = 0; i < g_total_reservas; i++) {
        if (strcmp(g_reservas[i].status, "confirmada") == 0 && !g_reservas[i].checkedIn) {
            printf("#%-4d | %-18s | %-8s | %-12s | Confirmada\n",
                   g_reservas[i].id, g_reservas[i].hospede, g_reservas[i].chale, g_reservas[i].checkin);
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

    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].id == id_reserva && !g_reservas[i].checkedIn) {
            g_reservas[i].checkedIn = 1;
            atualizar_status_chale(g_reservas[i].chale, 0, OCUPADO);

            char detalhe[80];
            snprintf(detalhe, sizeof(detalhe), "Reserva #%d (Hospede: %s)", g_reservas[i].id, g_reservas[i].hospede);
            adicionar_log_auditoria(sessao->usuario, "CHECKIN_REALIZADO", detalhe);

            printf("\n[SUCESSO] Check-in da Reserva #%d realizado com sucesso! Chale %s marcado como OCUPADO.\n",
                   g_reservas[i].id, g_reservas[i].chale);
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
        }
    }

    printf("\nReserva não encontrada ou já com check-in efetuado.\n");
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_consumo(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | LANCAR CONSUMO / EXTRATO DA CONTA\n");
    printf("================================================================================\n\n");

    printf("HOSPEDES ATUALMENTE HOSPEDADOS (COM CHECK-IN ATIVO):\n");
    int ativos = 0;
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].checkedIn && !g_reservas[i].checkedOut) {
            printf(" - [%s] Reserva #%d: %s\n", g_reservas[i].chale, g_reservas[i].id, g_reservas[i].hospede);
            ativos++;
        }
    }

    if (ativos == 0) {
        printf("Nenhum hóspede hospedado no momento. Realize um check-in primeiro.\n");
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

    printf("Descricao do consumo (ex: Agua e Refrigerante): ");
    if (scanf("%49[^\n]", item) != 1) {
        limpar_buffer();
        return ESTADO_MENU_PRINCIPAL;
    }
    limpar_buffer();

    printf("Valor em R$ (ex: 25.50): ");
    if (scanf("%f", &valor) != 1) {
        limpar_buffer();
        return ESTADO_MENU_PRINCIPAL;
    }
    limpar_buffer();

    char detalhe[80];
    snprintf(detalhe, sizeof(detalhe), "%s: %s (R$ %.2f)", chale, item, valor);
    adicionar_log_auditoria(sessao->usuario, "LANCAMENTO_CONSUMO", detalhe);

    printf("\n[SUCESSO] Consumo lançado com sucesso na conta do %s!\n", chale);
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_check_out(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | REALIZAR CHECK-OUT & FECHAMENTO\n");
    printf("================================================================================\n\n");

    printf("HOSPEDES COM CHECK-IN ATIVO (PENDENTES DE CHECK-OUT):\n");
    printf("%-5s | %-18s | %-8s | %-12s\n", "ID", "Hospede", "Chale", "Check-out Previsto");
    printf("--------------------------------------------------------------------------------\n");

    int ativos = 0;
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].checkedIn && !g_reservas[i].checkedOut) {
            printf("#%-4d | %-18s | %-8s | %-12s\n",
                   g_reservas[i].id, g_reservas[i].hospede, g_reservas[i].chale, g_reservas[i].checkout);
            ativos++;
        }
    }

    if (ativos == 0) {
        printf("Nenhum hóspede com check-in ativo no momento.\n");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }

    printf("\nDigite o ID da reserva para fechar e realizar check-out (ou 0 para voltar): ");
    int id_reserva = ler_opcao_segura();
    if (id_reserva == 0) return ESTADO_MENU_PRINCIPAL;

    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].id == id_reserva && g_reservas[i].checkedIn && !g_reservas[i].checkedOut) {
            g_reservas[i].checkedOut = 1;
            atualizar_status_chale(g_reservas[i].chale, 0, LIMPEZA);

            char detalhe[80];
            snprintf(detalhe, sizeof(detalhe), "Reserva #%d finalizada. %s enviado para LIMPEZA",
                     g_reservas[i].id, g_reservas[i].chale);
            adicionar_log_auditoria(sessao->usuario, "CHECKOUT_REALIZADO", detalhe);

            printf("\n[SUCESSO] Check-out concluído! Chalé %s alterado para LIMPEZA para equipe de governança.\n",
                   g_reservas[i].chale);
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
        }
    }

    printf("\nReserva não encontrada ou já finalizada.\n");
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_alterar_cancelar(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | ALTERAR / CANCELAR RESERVA\n");
    printf("================================================================================\n\n");

    printf("%-5s | %-18s | %-8s | %-12s | %s\n", "ID", "Hospede", "Chale", "Check-in", "Status");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < g_total_reservas; i++) {
        printf("#%-4d | %-18s | %-8s | %-12s | %s\n",
               g_reservas[i].id, g_reservas[i].hospede, g_reservas[i].chale, g_reservas[i].checkin, g_reservas[i].status);
    }

    printf("\nDigite o ID da reserva para cancelar (ou 0 para voltar): ");
    int id_reserva = ler_opcao_segura();
    if (id_reserva == 0) return ESTADO_MENU_PRINCIPAL;

    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].id == id_reserva) {
            if (strcmp(g_reservas[i].status, "cancelada") == 0) {
                printf("\nEsta reserva já se encontra cancelada.\n");
                pausar_sistema();
                return ESTADO_MENU_PRINCIPAL;
            }

            char motivo[60];
            printf("Motivo do cancelamento: ");
            if (scanf("%59[^\n]", motivo) != 1) {
                strcpy(motivo, "Desistência");
            }
            limpar_buffer();

            strcpy(g_reservas[i].status, "cancelada");
            atualizar_status_chale(g_reservas[i].chale, 0, LIVRE);

            char detalhe[80];
            snprintf(detalhe, sizeof(detalhe), "Res #%d - Motivo: %s", g_reservas[i].id, motivo);
            adicionar_log_auditoria(sessao->usuario, "CANCELA_RESERVA", detalhe);

            printf("\n[SUCESSO] Reserva #%d cancelada com sucesso. Chale %s liberado!\n",
                   g_reservas[i].id, g_reservas[i].chale);
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
        }
    }

    printf("\nReserva não encontrada.\n");
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_hospede(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | BUSCAR / CADASTRAR HOSPEDE\n");
    printf("================================================================================\n\n");

    printf("HOSPEDES CADASTRADOS:\n");
    printf("%-4s | %-20s | %-16s | %s\n", "ID", "Nome", "Documento/CPF", "Telefone");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < g_total_hospedes; i++) {
        printf("#%-3d | %-20s | %-16s | %s\n",
               g_hospedes[i].id, g_hospedes[i].nome, g_hospedes[i].documento, g_hospedes[i].telefone);
    }

    printf("\n[1] Cadastrar Novo Hóspede    [0] Voltar ao Menu\nOpcao > ");
    int op = ler_opcao_segura();

    if (op == 1 && g_total_hospedes < MAX_HOSPEDES) {
        Hospede novo;
        novo.id = g_total_hospedes + 1;

        printf("\nNome Completo: ");
        if (scanf("%49[^\n]", novo.nome) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        printf("CPF/Documento: ");
        if (scanf("%19[^\n]", novo.documento) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        printf("Telefone: ");
        if (scanf("%19[^\n]", novo.telefone) != 1) {
            limpar_buffer();
            return ESTADO_MENU_PRINCIPAL;
        }
        limpar_buffer();

        g_hospedes[g_total_hospedes++] = novo;

        char detalhe[80];
        snprintf(detalhe, sizeof(detalhe), "Hospede cadastrado: %s (CPF: %s)", novo.nome, novo.documento);
        adicionar_log_auditoria(sessao->usuario, "CADASTRO_HOSPEDE", detalhe);

        printf("\n[SUCESSO] Hóspede %s cadastrado com ID #%d!\n", novo.nome, novo.id);
        pausar_sistema();
    }

    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema processar_governanca(const SessaoUsuario *sessao) {
    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | GOVERNANCA & MANUTENCAO DE CHALES\n");
    printf("================================================================================\n\n");

    Chale *chales = obter_chales();
    for (int i = 0; i < NUM_CHALES; i++) {
        printf(" [%s] Tipo: %-10s | Status Atual: %s\n",
               chales[i].id, chales[i].tipo, nome_status(chales[i].status[0]));
    }

    printf("\n--------------------------------------------------------------------------------\n");
    printf("Deseja alterar o status de algum chalé? (ex: concluir limpeza)\n");
    printf("Digite o ID do chalé (ou '0' para voltar): ");

    char id_chale[8];
    if (scanf("%7s", id_chale) != 1) {
        limpar_buffer();
        return ESTADO_MENU_PRINCIPAL;
    }
    limpar_buffer();

    if (strcmp(id_chale, "0") == 0) return ESTADO_MENU_PRINCIPAL;

    printf("\nNovo status para %s:\n", id_chale);
    printf(" [1] LIVRE (Pronto para receber hóspedes)\n");
    printf(" [2] LIMPEZA\n");
    printf(" [3] MANUTENCAO\n");
    printf(" [4] OCUPADO\n");
    printf("Opção > ");

    int st = ler_opcao_segura();
    StatusChale novo_st;
    switch (st) {
        case 1: novo_st = LIVRE; break;
        case 2: novo_st = LIMPEZA; break;
        case 3: novo_st = MANUTENCAO; break;
        case 4: novo_st = OCUPADO; break;
        default:
            printf("\nOpção inválida.\n");
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

void executar_painel_recepcao(const SessaoUsuario *sessao) {
    EstadoSistema estado = ESTADO_MENU_PRINCIPAL;

    while (estado != ESTADO_SAIR) {
        switch (estado) {
            case ESTADO_MENU_PRINCIPAL:
                estado = processar_menu_principal(sessao);
                break;
            case ESTADO_DISPONIBILIDADE:
                estado = processar_disponibilidade(sessao);
                break;
            case ESTADO_MAPA_OCUPACAO:
                // Conexão com o código do Carlos
                estado = tela_mapa_ocupacao();
                break;
            case ESTADO_CHECK_IN:
                estado = processar_check_in(sessao);
                break;
            case ESTADO_CONSUMO:
                estado = processar_consumo(sessao);
                break;
            case ESTADO_CHECK_OUT:
                estado = processar_check_out(sessao);
                break;
            case ESTADO_ALTERAR_CANCELAR:
                estado = processar_alterar_cancelar(sessao);
                break;
            case ESTADO_HOSPEDE:
                estado = processar_hospede(sessao);
                break;
            case ESTADO_AUDITORIA:
                // Conexão com o código da Rayane
                estado = telaLogAuditoria();
                break;
            case ESTADO_GOVERNANCA:
                estado = processar_governanca(sessao);
                break;
            default:
                printf("\nEstado desconhecido. Retornando ao menu principal...\n");
                pausar_sistema();
                estado = ESTADO_MENU_PRINCIPAL;
                break;
        }
    }
}
