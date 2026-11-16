#include "telas.h"
#include <math.h>

/* LOGIN */
int exibir_tela_login(SessaoUsuario *sessao) {
    char usuario[32];
    char senha[32];

    while (1) {
        LIMPAR_TELA();
        printf(
            "================================================================================\n");
        printf(
            "                      RECANTO DO BOSQUE | SISTEMA DE GESTAO                     \n");
        printf(
            "================================================================================\n");
        printf("\nAutenticacao de funcionario (ou digite 'sair' para encerrar)\n\n");

        printf("Usuario: ");
        if (scanf("%31s", usuario) != 1) {
            return 0;
        }
        limpar_buffer();

        if (strcmp(usuario, "sair") == 0 || strcmp(usuario, "0") == 0) {
            return 0;
        }

        printf("Senha:   ");
        if (scanf("%31s", senha) != 1) {
            return 0;
        }
        limpar_buffer();

        if (autenticar_usuario(usuario, senha, sessao)) {
            printf("\nLogin realizado com sucesso!\n");
            printf("==============================================================================="
                   "=\n");
            if (sessao->perfil == PERFIL_RECEPCAO) {
                printf("ENCAMINHANDO PARA PAINEL DE RECEPCAO...\n");
            } else if (sessao->perfil == PERFIL_ADMIN) {
                printf("ENCAMINHANDO PARA PAINEL ADMINISTRATIVO...\n");
            } else {
                printf("ENCAMINHANDO PARA PAINEL DE GOVERNANCA...\n");
            }
            printf("==============================================================================="
                   "=\n");
            pausar_sistema();
            return 1;
        } else {
            printf("\n[ERRO] Usuario ou senha incorretos.\n");
            printf("Credenciais de teste:\n");
            printf(" - marcos.recepcao / 123456\n");
            printf(" - diretoria.admin / 123456\n");
            printf(" - ana.governanca  / 123456\n");
            pausar_sistema();
        }
    }
}

/* MAPA */
static const char *dia_semana_abrev(int wday) {
    static const char *dias[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};
    return dias[wday % 7];
}

static void imprimir_cabecalho(time_t data_inicial) {
    struct tm dia = *localtime(&data_inicial);

    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | MAPA DE OCUPACAO DA SEMANA                                   \n");
    printf("================================================================================\n\n");

    printf("%-8s| %-9s", "Chale", "Tipo");
    for (int i = 0; i < DIAS_VISIVEIS; i++) {
        printf("| %s %02d/%02d ", dia_semana_abrev(dia.tm_wday), dia.tm_mday, dia.tm_mon + 1);
        dia.tm_mday++;
        mktime(&dia);
    }
    printf("\n--------------------------------------------------------------------------------\n");
}

static void imprimir_linha_chale(const Chale *c, int offset) {
    printf("%-8s| %-9s", c->id, c->tipo);
    for (int i = 0; i < DIAS_VISIVEIS; i++) {
        int idx = offset + i;
        StatusChale s = idx == 0 ? c->status[0] : LIVRE;
        time_t agora = time(NULL);
        struct tm dia = *localtime(&agora);
        dia.tm_mday += idx;
        mktime(&dia);
        char data[12];
        strftime(data, sizeof(data), "%d/%m/%Y", &dia);
        int numero = data_numero(data);
        Reserva *r = obter_reservas();
        if (s == LIVRE) {
            for (int j = 0; j < obter_total_reservas(); j++) {
                if (!strcmp(r[j].chale, c->id) && strcmp(r[j].status, "cancelada") &&
                    !r[j].checkedOut && numero >= data_numero(r[j].checkin) &&
                    numero < data_numero(r[j].checkout)) {
                    s = r[j].checkedIn && !r[j].checkedOut ? OCUPADO : RESERVADO;
                    break;
                }
            }
        }
        printf("| %s ", rotulo_status(s));
    }
    printf("\n");
}

static void imprimir_legenda(void) {
    printf("--------------------------------------------------------------------------------\n");
    printf("Legenda: [LIVR] Livre | [RESV] Reservado | [OCUP] Ocupado | [LIMP] Limpeza | [MANU] "
           "Manutencao\n\n");
}

EstadoSistema exibir_tela_mapa(void) {
    Chale *chales = obter_chales();
    int total_chales = obter_total_chales();

    time_t data_inicial = time(NULL);
    int offset_dias = 0;
    int opcao;

    while (1) {
        LIMPAR_TELA();

        imprimir_cabecalho(data_inicial);
        for (int i = 0; i < total_chales; i++) {
            imprimir_linha_chale(&chales[i], offset_dias);
        }
        imprimir_legenda();

        printf("[1] Avancar 7 dias   [2] Voltar 7 dias   [0] Voltar ao Menu\n");
        printf("Opcao > ");

        opcao = ler_opcao_segura();

        switch (opcao) {
        case 1:
            if (offset_dias + 7 < TOTAL_DIAS) {
                offset_dias += 7;
                data_inicial += 7 * 24 * 60 * 60;
            } else {
                printf("\nLimite maximo de dias alcancado.\n");
                pausar_sistema();
            }
            break;
        case 2:
            if (offset_dias >= 7) {
                offset_dias -= 7;
                data_inicial -= 7 * 24 * 60 * 60;
            } else {
                printf("\nVoce ja esta visualizando a data de hoje.\n");
                pausar_sistema();
            }
            break;
        case 0:
            return ESTADO_MENU_PRINCIPAL;
        default:
            printf("\nOpcao invalida! Pressione ENTER para continuar...");
            getchar();
        }
    }
}

/* AUDITORIA */
#define LOGS_POR_PAGINA 5

static void mostrar_logs_paginados(LogAuditoria logs[], int quantidade, int pagina) {
    LIMPAR_TELA();

    int total_paginas = (quantidade + LOGS_POR_PAGINA - 1) / LOGS_POR_PAGINA;
    if (total_paginas == 0) {
        total_paginas = 1;
    }

    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | RELATORIO DE AUDITORIA DE OPERACOES (Pagina %d de %d)\n", pagina + 1,
           total_paginas);
    printf("================================================================================\n\n");

    printf("DATA/HORA        | USUARIO     | ACAO CRITICA       | DETALHES / MOTIVO\n");
    printf("--------------------------------------------------------------------------------\n");

    int inicio = pagina * LOGS_POR_PAGINA;
    int fim = inicio + LOGS_POR_PAGINA;
    if (fim > quantidade) {
        fim = quantidade;
    }

    for (int i = inicio; i < fim; i++) {
        printf("%-16s | %-11s | %-18s | %s\n", logs[i].datahora, logs[i].usuario, logs[i].acao,
               logs[i].detalhes);
    }

    if (quantidade == 0) {
        printf(" Nenhum registro de auditoria encontrado.\n");
    }

    printf("--------------------------------------------------------------------------------\n\n");
    printf(
        "[P] Proxima Pagina  |  [A] Pagina Anterior  |  [F] Filtrar por Usuario  |  [0] Voltar\n");
    printf("Opcao > ");
}

static void filtrar_por_usuario(LogAuditoria logs[], int quantidade) {
    char usuariobusca[24];
    int encontrou = 0;

    LIMPAR_TELA();
    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | FILTRO DE AUDITORIA\n");
    printf("================================================================================\n\n");

    printf("Digite o usuario a buscar: ");
    if (scanf("%23s", usuariobusca) != 1) {
        limpar_buffer();
        return;
    }
    limpar_buffer();

    printf("\nRESULTADOS PARA: %s\n\n", usuariobusca);
    printf("DATA/HORA        | USUARIO     | ACAO CRITICA       | DETALHES / MOTIVO\n");
    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < quantidade; i++) {
        if (strstr(logs[i].usuario, usuariobusca) != NULL) {
            printf("%-16s | %-11s | %-18s | %s\n", logs[i].datahora, logs[i].usuario, logs[i].acao,
                   logs[i].detalhes);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Nenhum registro encontrado para '%s'.\n", usuariobusca);
    }

    pausar_sistema();
}

EstadoSistema exibir_tela_auditoria(void) {
    LogAuditoria *logs = obter_logs_auditoria();
    int pagina = 0;
    char opcao[16];

    while (1) {
        int quantidade = obter_total_logs_auditoria();
        int total_paginas = (quantidade + LOGS_POR_PAGINA - 1) / LOGS_POR_PAGINA;
        if (total_paginas == 0) {
            total_paginas = 1;
        }

        mostrar_logs_paginados(logs, quantidade, pagina);
        if (scanf("%15s", opcao) != 1) {
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        if (opcao[0] == 'P' || opcao[0] == 'p') {
            if (pagina + 1 < total_paginas) {
                pagina++;
            } else {
                printf("\nNao ha mais paginas seguintes.\n");
                pausar_sistema();
            }
        } else if (opcao[0] == 'A' || opcao[0] == 'a') {
            if (pagina > 0) {
                pagina--;
            } else {
                printf("\nVoce ja esta na primeira pagina.\n");
                pausar_sistema();
            }
        } else if (opcao[0] == 'F' || opcao[0] == 'f') {
            filtrar_por_usuario(logs, quantidade);
        } else if (opcao[0] == '0') {
            return ESTADO_MENU_PRINCIPAL;
        } else {
            printf("\nOpcao invalida!\n");
            pausar_sistema();
        }
    }
}

/* OPERACOES */
static int linha(const char *titulo, char *buf, size_t n) {
    printf("%s", titulo);
    if (!fgets(buf, (int)n, stdin)) {
        if (n > 0) {
            buf[0] = '\0';
        }
        return 0;
    }
    size_t len = strlen(buf);
    if (len && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        limpar_buffer();
    }
    return *buf != '\0';
}
static void logar(const SessaoUsuario *s, const char *acao, int id) {
    char d[80];
    snprintf(d, sizeof(d), "Registro #%d", id);
    adicionar_log_auditoria(s->usuario, acao, d);
}
static void reservas_listar(void) {
    Reserva *r = obter_reservas();
    for (int i = 0; i < obter_total_reservas(); i++) {
        printf("#%d | %s | %s | %s a %s | %s | entrada: %s | saida: %s\n", r[i].id, r[i].hospede,
               r[i].chale, r[i].checkin, r[i].checkout, r[i].status, r[i].entrada_real,
               r[i].saida_real);
    }
}
static int dados_reserva(int *id, char *chale, char *a, char *b) {
    printf("ID do hospede cadastrado: ");
    *id = ler_opcao_segura();
    return linha("Chale (ex. CH-01): ", chale, 8) && linha("Entrada (DD/MM/AAAA): ", a, 12) &&
           linha("Saida (DD/MM/AAAA): ", b, 12);
}
static void resultado_reserva(int n) {
    if (n > 0) {
        printf("Reserva #%d salva.\n", n);
    } else if (n == -2) {
        printf("Bloqueio de Overbooking: Chale indisponivel para esta data.\n");
    } else {
        printf("Dados invalidos, cadastro inexistente ou limite atingido.\n");
    }
}
EstadoSistema exibir_tela_disponibilidade(const SessaoUsuario *s) {
    char a[12], b[12], ch[8];
    int hid;
    LIMPAR_TELA();
    puts("CONSULTA DE DISPONIBILIDADE");
    if (!linha("Entrada (DD/MM/AAAA): ", a, sizeof(a)) ||
        !linha("Saida (DD/MM/AAAA): ", b, sizeof(b)) || !periodo_valido(a, b)) {
        puts("Periodo invalido.");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    Chale *c = obter_chales();
    for (int i = 0; i < obter_total_chales(); i++) {
        if (chale_disponivel(c[i].id, a, b, 0)) {
            printf("%s | %s | capacidade %d | diaria R$ %.2f\n", c[i].id, c[i].tipo,
                   c[i].capacidade, c[i].tarifa);
        }
    }
    printf("[1] Criar reserva [0] Voltar: ");
    if (ler_opcao_segura() == 1) {
        Hospede *h = obter_hospedes();
        for (int i = 0; i < obter_total_hospedes(); i++) {
            printf("#%d %s | %s\n", h[i].id, h[i].nome, h[i].documento);
        }
        printf("ID do hospede: ");
        hid = ler_opcao_segura();
        if (linha("ID do chale: ", ch, sizeof(ch))) {
            int n = criar_reserva(hid, ch, a, b);
            resultado_reserva(n);
            if (n > 0) {
                logar(s, "NOVA_RESERVA", n);
            }
        }
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
EstadoSistema exibir_tela_checkin(const SessaoUsuario *s) {
    LIMPAR_TELA();
    reservas_listar();
    printf("ID da reserva para check-in (0 volta): ");
    int id = ler_opcao_segura();
    if (id && efetuar_checkin(id)) {
        puts("Check-in realizado. Chale ocupado.");
        logar(s, "CHECKIN_REALIZADO", id);
    } else if (id) {
        puts("Check-in indisponivel para esta reserva ou chale.");
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
EstadoSistema exibir_tela_consumo(const SessaoUsuario *s) {
    char item[50];
    float valor;
    LIMPAR_TELA();
    reservas_listar();
    printf("ID da estadia ativa: ");
    int id = ler_opcao_segura();
    if (!linha("Item: ", item, sizeof(item))) {
        return ESTADO_MENU_PRINCIPAL;
    }
    printf("Valor: ");
    if (scanf("%f", &valor) != 1) {
        limpar_buffer();
        return ESTADO_MENU_PRINCIPAL;
    }
    limpar_buffer();
    if (lancar_consumo(id, item, valor) > 0) {
        logar(s, "LANCAMENTO_CONSUMO", id);
        printf("Consumo registrado. Total da estadia: R$ %.2f\n", total_consumo(id));
    } else {
        puts("Estadia inativa ou valor invalido.");
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
EstadoSistema exibir_tela_checkout(const SessaoUsuario *s) {
    LIMPAR_TELA();
    reservas_listar();
    printf("ID da reserva: ");
    int id = ler_opcao_segura();
    Reserva *r = buscar_reserva_por_id(id);
    if (!r || !r->checkedIn || r->checkedOut) {
        puts("Estadia nao esta ativa.");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    int dias = data_numero(r->checkout) - data_numero(r->checkin);
    double extras = total_consumo(id), total = dias * r->tarifa + extras;
    printf("%d diaria(s) x R$ %.2f = R$ %.2f\nConsumo: R$ %.2f\nTotal: R$ %.2f\n", dias, r->tarifa,
           dias * r->tarifa, extras, total);
    ItemConsumo *it = obter_historico_consumo();
    for (int i = 0; i < obter_total_consumos(); i++) {
        if (it[i].reserva_id == id) {
            printf("  %s: R$ %.2f\n", it[i].item, it[i].valor);
        }
    }
    char metodo[24];
    if (!linha("Forma de pagamento (ou cancelar): ", metodo, sizeof(metodo)) ||
        !strcmp(metodo, "cancelar")) {
        return ESTADO_MENU_PRINCIPAL;
    }
    printf("Confirmar pagamento e check-out? [1] Sim: ");
    if (ler_opcao_segura() == 1 && efetuar_checkout(id)) {
        snprintf(r->pagamento, sizeof(r->pagamento), "%s", metodo);
        r->total_pago = total;
        logar(s, "CHECKOUT_REALIZADO", id);
        puts("Pagamento registrado. Chale enviado para limpeza.");
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
EstadoSistema exibir_tela_alterar_cancelar(const SessaoUsuario *s) {
    LIMPAR_TELA();
    reservas_listar();
    printf("ID da reserva: ");
    int id = ler_opcao_segura();
    printf("[1] Alterar [2] Cancelar: ");
    int op = ler_opcao_segura();
    if (op == 1) {
        int hid;
        char c[8], a[12], b[12];
        if (dados_reserva(&hid, c, a, b)) {
            int n = alterar_reserva(id, hid, c, a, b);
            resultado_reserva(n > 0 ? id : n);
            if (n > 0) {
                logar(s, "ALTERA_RESERVA", id);
            }
        }
    } else if (op == 2) {
        char motivo[60];
        if (linha("Motivo: ", motivo, sizeof(motivo))) {
            int n = cancelar_reserva(id);
            if (n == 1) {
                Reserva *r = buscar_reserva_por_id(id);
                snprintf(r->motivo, sizeof(r->motivo), "%s", motivo);
                adicionar_log_auditoria(s->usuario, "CANCELA_RESERVA", motivo);
                puts("Reserva cancelada.");
            } else {
                puts("Reserva inexistente, cancelada ou com estadia iniciada.");
            }
        }
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
EstadoSistema exibir_tela_hospedes(const SessaoUsuario *s) {
    LIMPAR_TELA();
    printf("[1] Cadastrar [2] Consultar por documento [3] Atualizar [4] Listar: ");
    int op = ler_opcao_segura();
    Hospede *h = obter_hospedes();
    if (op == 4) {
        for (int i = 0; i < obter_total_hospedes(); i++) {
            printf("#%d %s | %s | %s | %s | %s\n", h[i].id, h[i].nome, h[i].documento,
                   h[i].telefone, h[i].endereco, h[i].email);
        }
    } else if (op == 2) {
        char doc[20];
        if (linha("CPF/documento: ", doc, sizeof(doc))) {
            int achou = 0;
            for (int i = 0; i < obter_total_hospedes(); i++) {
                if (!strcmp(h[i].documento, doc)) {
                    printf("#%d %s | %s | %s | %s | %s\n", h[i].id, h[i].nome, h[i].documento,
                           h[i].telefone, h[i].endereco, h[i].email);
                    Reserva *r = obter_reservas();
                    for (int j = 0; j < obter_total_reservas(); j++) {
                        if (r[j].hospede_id == h[i].id) {
                            printf("Reserva #%d | %s | %s a %s | %s\n", r[j].id, r[j].chale,
                                   r[j].checkin, r[j].checkout, r[j].status);
                        }
                    }
                    achou = 1;
                }
            }
            if (!achou) {
                puts("Hospede nao localizado.");
            }
        }
    } else if (op == 1 || op == 3) {
        char nome[50], doc[20], tel[20], end[100], email[80];
        int id = 0;
        if (op == 3) {
            printf("ID: ");
            id = ler_opcao_segura();
            if (!buscar_hospede_por_id(id)) {
                puts("Hospede nao localizado.");
                pausar_sistema();
                return ESTADO_MENU_PRINCIPAL;
            }
        }
        if (!linha("Nome: ", nome, sizeof(nome)) || !linha("CPF/documento: ", doc, sizeof(doc))) {
            puts("Nome e documento sao obrigatorios.");
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
        }
        linha("Telefone: ", tel, sizeof(tel));
        linha("Endereco: ", end, sizeof(end));
        linha("E-mail: ", email, sizeof(email));
        if (op == 1) {
            id = cadastrar_hospede(nome, doc, tel);
        } else {
            for (int i = 0; i < obter_total_hospedes(); i++) {
                if (h[i].id != id && !strcmp(h[i].documento, doc)) {
                    id = -1;
                }
            }
            if (id > 0) {
                Hospede *p = buscar_hospede_por_id(id);
                snprintf(p->nome, sizeof(p->nome), "%s", nome);
                snprintf(p->documento, sizeof(p->documento), "%s", doc);
                snprintf(p->telefone, sizeof(p->telefone), "%s", tel);
                Reserva *r = obter_reservas();
                for (int j = 0; j < obter_total_reservas(); j++) {
                    if (r[j].hospede_id == id) {
                        snprintf(r[j].hospede, sizeof(r[j].hospede), "%s", nome);
                    }
                }
            }
        }
        Hospede *p = buscar_hospede_por_id(id);
        if (p) {
            snprintf(p->endereco, sizeof(p->endereco), "%s", end);
            snprintf(p->email, sizeof(p->email), "%s", email);
            logar(s, "CADASTRO_HOSPEDE", id);
            printf("Hospede #%d salvo.\n", id);
        } else {
            puts("Documento duplicado ou limite atingido.");
        }
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
EstadoSistema exibir_tela_governanca(const SessaoUsuario *s) {
    LIMPAR_TELA();
    exibir_tela_mapa();
    char id[8];
    if (!linha("Chale para atualizar (0 volta): ", id, sizeof(id)) || !strcmp(id, "0")) {
        return ESTADO_MENU_PRINCIPAL;
    }
    Chale *c = buscar_chale_por_id(id);
    if (!c || c->status[0] == OCUPADO) {
        puts("Chale inexistente ou ocupado.");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    printf("[1] Livre [2] Limpeza [3] Manutencao: ");
    int op = ler_opcao_segura();
    if (op >= 1 && op <= 3) {
        atualizar_status_chale(id, 0, op == 1 ? LIVRE : op == 2 ? LIMPEZA : MANUTENCAO);
        adicionar_log_auditoria(s->usuario, "STATUS_CHALE", id);
        puts("Status atualizado.");
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
EstadoSistema exibir_tela_chales(const SessaoUsuario *s) {
    if (!s || s->perfil != PERFIL_ADMIN) {
        return ESTADO_MENU_PRINCIPAL;
    }
    LIMPAR_TELA();
    Chale *c = obter_chales();
    for (int i = 0; i < obter_total_chales(); i++) {
        printf("%s | %s | %d pessoas | R$ %.2f\n", c[i].id, c[i].tipo, c[i].capacidade,
               c[i].tarifa);
    }
    char id[8], tipo[16];
    if (!linha("ID para cadastrar ou atualizar: ", id, sizeof(id))) {
        return ESTADO_MENU_PRINCIPAL;
    }
    Chale *p = buscar_chale_por_id(id);
    if (!linha("Tipo: ", tipo, sizeof(tipo))) {
        return ESTADO_MENU_PRINCIPAL;
    }
    printf("Capacidade: ");
    int cap = ler_opcao_segura();
    printf("Tarifa diaria: ");
    double tarifa;
    if (scanf("%lf", &tarifa) != 1) {
        limpar_buffer();
        return ESTADO_MENU_PRINCIPAL;
    }
    limpar_buffer();
    if (cap > 0 && isfinite(tarifa) && tarifa >= 0) {
        if (p) {
            snprintf(p->tipo, sizeof(p->tipo), "%s", tipo);
            p->capacidade = cap;
            p->tarifa = tarifa;
        } else if (!cadastrar_chale(id, tipo, cap, tarifa)) {
            puts("Nao foi possivel cadastrar o chale.");
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
        }
        adicionar_log_auditoria(s->usuario, "ALTERA_CHALE", id);
        puts("Chale salvo.");
    } else {
        puts("Dados invalidos.");
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
EstadoSistema exibir_tela_relatorio(const SessaoUsuario *s) {
    if (!s || s->perfil != PERFIL_ADMIN) {
        return ESTADO_MENU_PRINCIPAL;
    }
    char a[12], b[12];
    LIMPAR_TELA();
    if (!linha("Inicio (DD/MM/AAAA): ", a, sizeof(a)) ||
        !linha("Fim exclusivo (DD/MM/AAAA): ", b, sizeof(b)) || !periodo_valido(a, b)) {
        puts("Periodo invalido.");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    int inicio = data_numero(a), fim = data_numero(b), total = 0, registros = 0;
    double faturamento = 0;
    Reserva *r = obter_reservas();
    Chale *c = obter_chales();
    puts("Chale   | Diarias reservadas | Ocupacao");
    for (int j = 0; j < obter_total_chales(); j++) {
        int n = 0;
        for (int i = 0; i < obter_total_reservas(); i++) {
            if (!strcmp(r[i].chale, c[j].id) && strcmp(r[i].status, "cancelada")) {
                int x = data_numero(r[i].checkin), y = data_numero(r[i].checkout);
                int z = (y < fim ? y : fim) - (x > inicio ? x : inicio);
                if (z > 0) {
                    n += z;
                    registros++;
                }
            }
        }
        total += n;
        printf("%-7s | %18d | %7.2f%%\n", c[j].id, n, 100.0 * n / (fim - inicio));
    }
    for (int i = 0; i < obter_total_reservas(); i++) {
        if (r[i].checkedOut) {
            char data[11];
            memcpy(data, r[i].saida_real, 10);
            data[10] = 0;
            int d = data_numero(data);
            if (d >= inicio && d < fim) {
                faturamento += r[i].total_pago;
            }
        }
    }
    if (!registros) {
        puts("Nenhum registro de ocupacao encontrado para o periodo.");
    }
    printf("Taxa geral (reservas): %.2f%% | Pagamentos recebidos: R$ %.2f\n",
           100.0 * total / ((fim - inicio) * obter_total_chales()), faturamento);
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

/* RECEPCAO */
static EstadoSistema processar_menu_principal(const SessaoUsuario *sessao) {
    LIMPAR_TELA();

    char data_str[24];
    obter_data_hora_atual(data_str, sizeof(data_str));

    printf("================================================================================\n");
    printf(" RECANTO BOSQUE | Operador: %-20s | Data: %s\n", sessao->nome, data_str);
    printf("================================================================================\n");
    printf("\nRESUMO DO DIA:\n");
    printf(" [ Check-ins Pendentes: %d ]  [ Check-outs Pendentes: %d ]  [ Chales Livres: %d ]\n",
           contar_checkins_pendentes(), contar_checkouts_pendentes(), contar_chales_livres_hoje());
    printf("\n--------------------------------------------------------------------------------\n");
    printf("MENU PRINCIPAL:\n\n");
    printf(" [1] Consultar Disponibilidade & Nova Reserva\n");
    printf(" [2] Mapa de Ocupação (Visão Geral - Carlos)\n");
    printf(" [3] Realizar Check-in\n");
    printf(" [4] Lançar Consumo / Extrato da Conta\n");
    printf(" [5] Realizar Check-out & Fechamento\n");
    printf(" [6] Alterar / Cancelar Reserva\n");
    printf(" [7] Buscar / Cadastrar Hóspede\n");
    if (sessao->perfil == PERFIL_ADMIN) {
        printf(" [8] Relatório de Auditoria de Operações\n");
    }
    printf(" [9] Governança / Manutenção de Chalés\n");
    if (sessao->perfil == PERFIL_ADMIN) {
        printf(" [10] Cadastro de chales\n [11] Relatorio de ocupacao e faturamento\n");
    }
    printf(" [0] Sair (Logoff)\n");
    printf("\n--------------------------------------------------------------------------------\n");
    printf("Opção desejada > ");

    int opcao = ler_opcao_segura();

    switch (opcao) {
    case 1:
        return ESTADO_DISPONIBILIDADE;
    case 2:
        return ESTADO_MAPA_OCUPACAO;
    case 3:
        return ESTADO_CHECK_IN;
    case 4:
        return ESTADO_CONSUMO;
    case 5:
        return ESTADO_CHECK_OUT;
    case 6:
        return ESTADO_ALTERAR_CANCELAR;
    case 7:
        return ESTADO_HOSPEDE;
    case 8:
        return sessao->perfil == PERFIL_ADMIN ? ESTADO_AUDITORIA : ESTADO_MENU_PRINCIPAL;
    case 9:
        return ESTADO_GOVERNANCA;
    case 10:
        return sessao->perfil == PERFIL_ADMIN ? ESTADO_CHALES : ESTADO_MENU_PRINCIPAL;
    case 11:
        return sessao->perfil == PERFIL_ADMIN ? ESTADO_RELATORIO : ESTADO_MENU_PRINCIPAL;
    case 0:
        return ESTADO_SAIR;
    default:
        printf("\nOpção inválida! Digite apenas números do menu.\n");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
}

void executar_painel_recepcao(const SessaoUsuario *sessao) {
    EstadoSistema estado = ESTADO_MENU_PRINCIPAL;

    while (estado != ESTADO_SAIR) {
        switch (estado) {
        case ESTADO_MENU_PRINCIPAL:
            estado = processar_menu_principal(sessao);
            break;
        case ESTADO_DISPONIBILIDADE:
            estado = exibir_tela_disponibilidade(sessao);
            break;
        case ESTADO_MAPA_OCUPACAO:
            // Conexão com o mapa desenvolvido por Carlos
            estado = exibir_tela_mapa();
            break;
        case ESTADO_CHECK_IN:
            estado = exibir_tela_checkin(sessao);
            break;
        case ESTADO_CONSUMO:
            estado = exibir_tela_consumo(sessao);
            break;
        case ESTADO_CHECK_OUT:
            estado = exibir_tela_checkout(sessao);
            break;
        case ESTADO_ALTERAR_CANCELAR:
            estado = exibir_tela_alterar_cancelar(sessao);
            break;
        case ESTADO_HOSPEDE:
            estado = exibir_tela_hospedes(sessao);
            break;
        case ESTADO_CHALES:
            estado = exibir_tela_chales(sessao);
            break;
        case ESTADO_RELATORIO:
            estado = exibir_tela_relatorio(sessao);
            break;
        case ESTADO_AUDITORIA:
            // Conexão com o relatório de auditoria desenvolvido por Rayane
            estado =
                sessao->perfil == PERFIL_ADMIN ? exibir_tela_auditoria() : ESTADO_MENU_PRINCIPAL;
            break;
        case ESTADO_GOVERNANCA:
            estado = exibir_tela_governanca(sessao);
            break;
        default:
            printf("\nEstado não reconhecido. Retornando ao menu principal...\n");
            pausar_sistema();
            estado = ESTADO_MENU_PRINCIPAL;
            break;
        }
    }
}
