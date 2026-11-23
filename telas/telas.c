#include "telas.h"
#include <math.h>

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

/* Identificacao visual dos chales, sem expor os codigos internos. */
static int numero_chale(const char *codigo) {
    Chale *chales = obter_chales();
    for (int i = 0; i < obter_total_chales(); i++) {
        if (strcmp(chales[i].id, codigo) == 0) {
            return i + 1;
        }
    }
    return 0;
}

static int permite_recepcao(const SessaoUsuario *sessao) {
    return sessao && (sessao->perfil == PERFIL_RECEPCAO || sessao->perfil == PERFIL_ADMIN);
}

static int permite_governanca(const SessaoUsuario *sessao) {
    return permite_recepcao(sessao) || (sessao && sessao->perfil == PERFIL_GOVERNANCA);
}

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
            pausar_sistema();
        }
    }
}

/* MAPA */
static const char *dia_semana_abrev(int wday) {
    static const char *dias[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};
    return dias[wday % 7];
}

static int numero_data_real(const char *data_hora) {
    char data[11];
    if (!data_hora || strlen(data_hora) < 10) {
        return -1;
    }
    memcpy(data, data_hora, 10);
    data[10] = '\0';
    return data_numero(data);
}

static StatusChale status_no_dia(const Chale *chale, int dia, int hoje) {
    if (dia == hoje && chale->status[0] != LIVRE) {
        return chale->status[0];
    }
    Reserva *reservas = obter_reservas();
    for (int i = 0; i < obter_total_reservas(); i++) {
        Reserva *reserva = &reservas[i];
        if (strcmp(reserva->chale, chale->id) != 0 ||
            strcmp(reserva->status, "cancelada") == 0) {
            continue;
        }
        if (reserva->checkedIn) {
            int entrada = numero_data_real(reserva->entrada_real);
            int saida = reserva->checkedOut ? numero_data_real(reserva->saida_real) : hoje + 1;
            if (saida == entrada) {
                saida++;
            }
            if (entrada >= 0 && dia >= entrada && dia < saida) {
                return OCUPADO;
            }
        }
        if (!reserva->checkedOut && dia >= hoje && dia >= data_numero(reserva->checkin) &&
            dia < data_numero(reserva->checkout)) {
            return RESERVADO;
        }
    }
    return LIVRE;
}

static void imprimir_mes(const struct tm *mes, int hoje) {
    struct tm primeiro = *mes;
    primeiro.tm_mday = 1;
    primeiro.tm_hour = 12;
    primeiro.tm_min = 0;
    primeiro.tm_sec = 0;
    primeiro.tm_isdst = -1;
    mktime(&primeiro);
    struct tm proximo = primeiro;
    proximo.tm_mon++;
    mktime(&proximo);
    proximo.tm_mday = 0;
    mktime(&proximo);
    int dias_no_mes = proximo.tm_mday;
    char data[12];
    strftime(data, sizeof(data), "%d/%m/%Y", &primeiro);
    int primeiro_numero = data_numero(data);
    Chale *chales = obter_chales();

    printf("========================================================================\n");
    printf(" RECANTO BOSQUE | MAPA DE OCUPACAO | MES %02d/%04d\n", primeiro.tm_mon + 1,
           primeiro.tm_year + 1900);
    printf("========================================================================\n");
    for (int inicio = 1; inicio <= dias_no_mes; inicio += DIAS_VISIVEIS) {
        int quantidade = dias_no_mes - inicio + 1;
        if (quantidade > DIAS_VISIVEIS) {
            quantidade = DIAS_VISIVEIS;
        }
        struct tm dia = primeiro;
        dia.tm_mday = inicio;
        mktime(&dia);
        printf("\n%-8s| %-9s", "Chale", "Tipo");
        for (int i = 0; i < quantidade; i++) {
            printf("| %s %02d/%02d ", dia_semana_abrev(dia.tm_wday), dia.tm_mday, dia.tm_mon + 1);
            dia.tm_mday++;
            mktime(&dia);
        }
        putchar('\n');
        for (int i = 0; i < obter_total_chales(); i++) {
            printf("Chale %-2d| %-9s", numero_chale(chales[i].id), chales[i].tipo);
            for (int d = 0; d < quantidade; d++) {
                StatusChale estado = status_no_dia(&chales[i], primeiro_numero + inicio + d - 1, hoje);
                printf("| %s ", rotulo_status(estado));
            }
            putchar('\n');
        }
    }
    printf("\nLegenda: [LIVR] Livre | [RESV] Reservado | [OCUP] Ocupado | ");
    printf("[LIMP] Limpeza | [MANU] Manutencao\n");
    printf("Limpeza e manutencao mostram apenas o estado atual.\n\n");
}

EstadoSistema exibir_tela_mapa(const SessaoUsuario *sessao) {
    if (!permite_governanca(sessao)) {
        return ESTADO_MENU_PRINCIPAL;
    }
    time_t agora = time(NULL);
    struct tm *data_atual = localtime(&agora);
    if (!data_atual) {
        return ESTADO_MENU_PRINCIPAL;
    }
    struct tm mes_atual = *data_atual;
    struct tm mes = mes_atual;
    mes.tm_mday = 1;
    mes.tm_hour = 12;
    mes.tm_isdst = -1;
    char data_hoje[12];
    if (!strftime(data_hoje, sizeof(data_hoje), "%d/%m/%Y", data_atual)) {
        return ESTADO_MENU_PRINCIPAL;
    }
    int hoje = data_numero(data_hoje);
    while (1) {
        LIMPAR_TELA();
        imprimir_mes(&mes, hoje);
        printf("[1] Proximo mes   [2] Mes anterior   [3] Mes atual   [0] Voltar ao Menu\n");
        printf("Opcao > ");
        int opcao = ler_opcao_segura();
        if (opcao == 0) {
            return ESTADO_MENU_PRINCIPAL;
        }
        if (opcao == 3) {
            mes = mes_atual;
            mes.tm_mday = 1;
        } else if ((opcao == 1 && (mes.tm_year + 1900 < 9999 || mes.tm_mon < 11)) ||
                   (opcao == 2 && (mes.tm_year + 1900 > 1900 || mes.tm_mon > 0))) {
            mes.tm_mon += opcao == 1 ? 1 : -1;
            mes.tm_isdst = -1;
            mktime(&mes);
        } else {
            puts("Opcao invalida ou limite de datas atingido.");
            pausar_sistema();
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

EstadoSistema exibir_tela_auditoria(const SessaoUsuario *sessao) {
    if (!sessao || sessao->perfil != PERFIL_ADMIN) {
        return ESTADO_MENU_PRINCIPAL;
    }
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
        int proximo = getchar();
        if (proximo != '\n' && proximo != EOF) {
            limpar_buffer();
            buf[0] = '\0';
            printf("Entrada muito longa. Tente novamente.\n");
            return 0;
        }
    }
    return *buf != '\0';
}
/* A mascara aceita oito digitos e monta a data sem exigir as barras. */
static int ler_data(const char *titulo, char data[12]) {
    char digitos[9] = {0};
    int quantidade = 0;
#ifdef _WIN32
    int interativo = _isatty(_fileno(stdin));
#else
    int interativo = isatty(STDIN_FILENO);
    struct termios original;
    if (interativo) {
        struct termios modo;
        if (tcgetattr(STDIN_FILENO, &original) != 0) {
            interativo = 0;
        } else {
            modo = original;
            modo.c_lflag &= (tcflag_t)~(ICANON | ECHO | ISIG);
            modo.c_cc[VMIN] = 1;
            modo.c_cc[VTIME] = 0;
            if (tcsetattr(STDIN_FILENO, TCSANOW, &modo) != 0) {
                interativo = 0;
            }
        }
    }
#endif
    if (!interativo) {
        char entrada[32];
        if (!linha(titulo, entrada, sizeof(entrada))) {
            return 0;
        }
        size_t tamanho = strlen(entrada);
        if (tamanho == 10 && entrada[2] == '/' && entrada[5] == '/') {
            snprintf(data, 12, "%s", entrada);
            return 1;
        }
        if (tamanho != 8 || strspn(entrada, "0123456789") != 8) {
            return 0;
        }
        snprintf(data, 12, "%.2s/%.2s/%.4s", entrada, entrada + 2, entrada + 4);
        return 1;
    }

    printf("%s", titulo);
    fflush(stdout);
    for (;;) {
#ifdef _WIN32
        int c = _getch();
#else
        int c = getchar();
#endif
        if (c == EOF || c == '\n' || c == '\r' || c == 3) {
            if (c == 3) {
                quantidade = 0;
            }
            break;
        }
        if (c == 8 || c == 127) {
            if (quantidade > 0) {
                if (quantidade == 2 || quantidade == 4) {
                    printf("\b \b");
                }
                printf("\b \b");
                digitos[--quantidade] = '\0';
                fflush(stdout);
            }
        } else if (c >= '0' && c <= '9' && quantidade < 8) {
            digitos[quantidade++] = (char)c;
            putchar(c);
            if (quantidade == 2 || quantidade == 4) {
                putchar('/');
            }
            fflush(stdout);
        }
    }
#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &original);
#endif
    putchar('\n');
    if (quantidade != 8) {
        return 0;
    }
    snprintf(data, 12, "%.2s/%.2s/%.4s", digitos, digitos + 2, digitos + 4);
    return 1;
}

static void logar(const SessaoUsuario *sessao, const char *acao, int registro) {
    char detalhes[80];
    if (strcmp(acao, "CADASTRO_HOSPEDE") == 0) {
        Hospede *hospede = buscar_hospede_por_id(registro);
        snprintf(detalhes, sizeof(detalhes), "Hospede: %s",
                 hospede ? hospede->nome : "desconhecido");
    } else {
        Reserva *reserva = buscar_reserva_por_id(registro);
        if (reserva) {
            snprintf(detalhes, sizeof(detalhes), "%s | Chale %d | %s", reserva->hospede,
                     numero_chale(reserva->chale), reserva->checkin);
        } else {
            snprintf(detalhes, sizeof(detalhes), "Operacao realizada");
        }
    }
    adicionar_log_auditoria(sessao->usuario, acao, detalhes);
}

/* Os numeros exibidos sao apenas opcoes da lista atual. */
static Reserva *selecionar_reserva(const char *titulo) {
    Reserva *reservas = obter_reservas();
    int total = obter_total_reservas();

    printf("======================================================================\n");
    printf(" RECANTO BOSQUE | %s\n", titulo);
    printf("======================================================================\n\n");

    if (total == 0) {
        puts("Nenhuma reserva cadastrada nesta execucao.");
        return NULL;
    }

    puts("Selecione a reserva desejada:\n");
    for (int i = 0; i < total; i++) {
        Reserva *reserva = &reservas[i];
        const char *situacao = reserva->status;
        if (reserva->checkedOut) {
            situacao = "Finalizada";
        } else if (reserva->checkedIn) {
            situacao = "Hospedado";
        }
        printf(" [%d] %s\n", i + 1, reserva->hospede);
        printf("     Chale %d  |  %s ate %s  |  %s\n", numero_chale(reserva->chale),
               reserva->checkin, reserva->checkout, situacao);
        puts(" --------------------------------------------------------------------");
    }
    printf("\nDigite o numero da reserva ou 0 para voltar: ");
    int opcao = ler_opcao_segura();
    if (opcao == 0) {
        return NULL;
    }
    if (opcao < 1 || opcao > total) {
        puts("Opcao invalida.");
        return NULL;
    }
    return &reservas[opcao - 1];
}

static Hospede *selecionar_hospede(void) {
    Hospede *hospedes = obter_hospedes();
    int total = obter_total_hospedes();
    for (int i = 0; i < total; i++) {
        printf("ID %d | %s\n", hospedes[i].id, hospedes[i].nome);
    }
    printf("Informe o ID do hospede (0 para voltar): ");
    int id = ler_opcao_segura();
    if (id <= 0) {
        return NULL;
    }
    Hospede *hospede = buscar_hospede_por_id(id);
    if (!hospede) {
        puts("Hospede nao localizado.");
    }
    return hospede;
}

static Chale *selecionar_chale(const char *entrada, const char *saida, int ignorar) {
    Chale *chales = obter_chales();
    int total = obter_total_chales();
    int disponiveis[MAX_CHALES];
    int quantidade = 0;
    for (int i = 0; i < total; i++) {
        if (!entrada || chale_disponivel(chales[i].id, entrada, saida, ignorar)) {
            disponiveis[quantidade++] = i;
            printf("[%d] Chale %d | %s | capacidade %d | diaria R$ %.2f\n", quantidade, i + 1,
                   chales[i].tipo, chales[i].capacidade, chales[i].tarifa);
        }
    }
    if (!quantidade) {
        puts("Nenhum chale disponivel.");
        return NULL;
    }
    printf("Escolha um chale pelo numero da lista (0 para voltar): ");
    int opcao = ler_opcao_segura();
    if (opcao < 1 || opcao > quantidade) {
        return NULL;
    }
    return &chales[disponiveis[opcao - 1]];
}

static int dados_reserva(int *id, char *chale, char *a, char *b) {
    Hospede *h = selecionar_hospede();
    if (!h) {
        return 0;
    }
    *id = h->id;
    if (!ler_data("Entrada (DD/MM/AAAA): ", a) || !ler_data("Saida (DD/MM/AAAA): ", b) ||
        !periodo_reserva_valido(a, b)) {
        puts("Periodo invalido: a entrada nao pode ser anterior a hoje e a saida deve ser "
             "posterior.");
        return 0;
    }
    Chale *c = selecionar_chale(NULL, NULL, 0);
    if (!c) {
        return 0;
    }
    snprintf(chale, 8, "%s", c->id);
    return 1;
}
static void resultado_reserva(int resultado) {
    if (resultado > 0) {
        puts("Reserva salva.");
    } else if (resultado == -2) {
        puts("Bloqueio de Overbooking: Chale indisponivel para esta data.");
    } else {
        puts("Dados invalidos ou limite atingido.");
    }
}
EstadoSistema exibir_tela_disponibilidade(const SessaoUsuario *s) {
    if (!permite_recepcao(s)) {
        return ESTADO_MENU_PRINCIPAL;
    }
    char a[12], b[12];
    LIMPAR_TELA();
    puts("CONSULTA DE DISPONIBILIDADE");
    if (!ler_data("Entrada (DD/MM/AAAA): ", a) ||
        !ler_data("Saida (DD/MM/AAAA): ", b) || !periodo_reserva_valido(a, b)) {
        puts("Periodo invalido: a entrada nao pode ser anterior a hoje e a saida deve ser "
             "posterior.");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    Chale *c = selecionar_chale(a, b, 0);
    if (c) {
        Hospede *h = selecionar_hospede();
        if (h) {
            int resultado = criar_reserva(h->id, c->id, a, b);
            resultado_reserva(resultado);
            if (resultado > 0) {
                logar(s, "NOVA_RESERVA", resultado);
            }
        }
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
EstadoSistema exibir_tela_checkin(const SessaoUsuario *s) {
    if (!permite_recepcao(s)) {
        return ESTADO_MENU_PRINCIPAL;
    }
    LIMPAR_TELA();
    Reserva *reserva = selecionar_reserva("REALIZAR CHECK-IN");
    if (!reserva) {
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    int id = reserva->id;
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
    if (!permite_recepcao(s)) {
        return ESTADO_MENU_PRINCIPAL;
    }
    char item[50];
    float valor;
    LIMPAR_TELA();
    Reserva *reserva = selecionar_reserva("LANCAR CONSUMO");
    if (!reserva) {
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    int id = reserva->id;
    if (!reserva || !reserva->checkedIn || reserva->checkedOut) {
        puts("Reserva nao encontrada ou estadia nao esta ativa.");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
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
    if (!permite_recepcao(s)) {
        return ESTADO_MENU_PRINCIPAL;
    }
    LIMPAR_TELA();
    Reserva *r = selecionar_reserva("REALIZAR CHECK-OUT");
    if (!r) {
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    int id = r->id;
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
    if (!permite_recepcao(s)) {
        return ESTADO_MENU_PRINCIPAL;
    }
    LIMPAR_TELA();
    Reserva *reserva = selecionar_reserva("ALTERAR OU CANCELAR RESERVA");
    if (!reserva) {
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    int id = reserva->id;
    printf("[1] Alterar [2] Cancelar");
    if (s->perfil == PERFIL_ADMIN) {
        printf(" [3] Excluir reserva cancelada");
    }
    printf(": ");
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
    } else if (op == 3 && s->perfil == PERFIL_ADMIN) {
        printf("Excluir reserva cancelada? [1] Sim: ");
        if (ler_opcao_segura() == 1 && excluir_reserva(id)) {
            adicionar_log_auditoria(s->usuario, "EXCLUI_RESERVA", "Reserva cancelada excluida");
            puts("Reserva excluida.");
        } else {
            puts("Exclusao cancelada ou reserva nao elegivel.");
        }
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
static EstadoSistema operar_hospedes(const SessaoUsuario *s, int op) {
    if (!permite_recepcao(s)) {
        return ESTADO_MENU_PRINCIPAL;
    }
    LIMPAR_TELA();
    if (op == 1) {
        puts("CADASTRAR HOSPEDE\n");
    } else if (op == 2) {
        puts("BUSCAR HOSPEDE\n");
    }
    Hospede *h = obter_hospedes();
    if (op == 5 && s->perfil == PERFIL_ADMIN) {
        Hospede *escolhido = selecionar_hospede();
        if (escolhido) {
            int id = escolhido->id;
            printf("Excluir hospede sem reservas? [1] Sim: ");
            if (ler_opcao_segura() == 1 && excluir_hospede(id)) {
                adicionar_log_auditoria(s->usuario, "EXCLUI_HOSPEDE", "Cadastro excluido");
                puts("Hospede excluido.");
            } else {
                puts("Exclusao cancelada ou hospede possui reservas.");
            }
        }
    } else if (op == 4) {
        for (int i = 0; i < obter_total_hospedes(); i++) {
            printf("ID %d | %s\n", h[i].id, h[i].nome);
        }
    } else if (op == 2) {
        char doc[20];
        if (linha("CPF/documento: ", doc, sizeof(doc))) {
            int achou = 0;
            for (int i = 0; i < obter_total_hospedes(); i++) {
                if (!strcmp(h[i].documento, doc)) {
                    printf("%s | %s | %s | %s | %s\n", h[i].nome, h[i].documento, h[i].telefone,
                           h[i].endereco, h[i].email);
                    Reserva *r = obter_reservas();
                    for (int j = 0; j < obter_total_reservas(); j++) {
                        if (r[j].hospede_id == h[i].id) {
                            printf("Chale %d | %s a %s | %s\n", numero_chale(r[j].chale),
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
            Hospede *escolhido = selecionar_hospede();
            id = escolhido ? escolhido->id : 0;
            if (!escolhido) {
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
            puts("Hospede salvo.");
        } else {
            puts("Documento duplicado ou limite atingido.");
        }
    }
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}
EstadoSistema exibir_tela_hospedes(const SessaoUsuario *s) {
    if (!permite_recepcao(s)) {
        return ESTADO_MENU_PRINCIPAL;
    }
    LIMPAR_TELA();
    printf("[1] Atualizar [2] Listar");
    if (s->perfil == PERFIL_ADMIN) {
        printf(" [3] Excluir");
    }
    printf(" [0] Voltar: ");
    int escolha = ler_opcao_segura();
    if (escolha == 1) {
        return operar_hospedes(s, 3);
    }
    if (escolha == 2) {
        return operar_hospedes(s, 4);
    }
    if (escolha == 3 && s->perfil == PERFIL_ADMIN) {
        return operar_hospedes(s, 5);
    }
    return ESTADO_MENU_PRINCIPAL;
}

EstadoSistema exibir_tela_governanca(const SessaoUsuario *s) {
    if (!permite_governanca(s)) {
        return ESTADO_MENU_PRINCIPAL;
    }
    LIMPAR_TELA();
    exibir_tela_mapa(s);
    Chale *c = selecionar_chale(NULL, NULL, 0);
    if (!c) {
        return ESTADO_MENU_PRINCIPAL;
    }
    if (!c || c->status[0] == OCUPADO) {
        puts("Chale inexistente ou ocupado.");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    printf("[1] Livre [2] Limpeza [3] Manutencao: ");
    int op = ler_opcao_segura();
    if (op >= 1 && op <= 3) {
        atualizar_status_chale(c->id, 0, op == 1 ? LIVRE : op == 2 ? LIMPEZA : MANUTENCAO);
        adicionar_log_auditoria(s->usuario, "STATUS_CHALE", "Status atualizado");
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
    printf("[1] Cadastrar chale [2] Atualizar chale [3] Excluir chale [0] Voltar: ");
    int escolha = ler_opcao_segura();
    Chale *p = NULL;
    char id[8], tipo[16];
    if (escolha == 3) {
        p = selecionar_chale(NULL, NULL, 0);
        if (p) {
            char codigo[8];
            snprintf(codigo, sizeof(codigo), "%s", p->id);
            printf("Excluir chale sem reservas? [1] Sim: ");
            if (ler_opcao_segura() == 1 && excluir_chale(codigo)) {
                adicionar_log_auditoria(s->usuario, "EXCLUI_CHALE", "Chale excluido");
                puts("Chale excluido.");
            } else {
                puts("Exclusao cancelada, chale vinculado ou ultimo chale.");
            }
            pausar_sistema();
        }
        return ESTADO_MENU_PRINCIPAL;
    }
    if (escolha == 2) {
        p = selecionar_chale(NULL, NULL, 0);
        if (!p) {
            return ESTADO_MENU_PRINCIPAL;
        }
        snprintf(id, sizeof(id), "%s", p->id);
    } else if (escolha == 1) {
        int numero = 1;
        do {
            snprintf(id, sizeof(id), "CH-%03d", numero++);
        } while (numero <= MAX_CHALES + 1 && buscar_chale_por_id(id));
    } else {
        return ESTADO_MENU_PRINCIPAL;
    }
    printf("Tipo do chale:\n");
    printf(" [1] Standard\n [2] Familia\n [3] Luxo\n [0] Voltar\n");
    printf("Escolha uma opcao: ");
    int tipo_opcao = ler_opcao_segura();
    switch (tipo_opcao) {
    case 1:
        snprintf(tipo, sizeof(tipo), "Standard");
        break;
    case 2:
        snprintf(tipo, sizeof(tipo), "Familia");
        break;
    case 3:
        snprintf(tipo, sizeof(tipo), "Luxo");
        break;
    case 0:
        return ESTADO_MENU_PRINCIPAL;
    default:
        puts("Tipo invalido.");
        pausar_sistema();
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
        adicionar_log_auditoria(s->usuario, "ALTERA_CHALE", "Cadastro atualizado");
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
    if (!ler_data("Inicio (DD/MM/AAAA): ", a) ||
        !ler_data("Fim exclusivo (DD/MM/AAAA): ", b) || !periodo_valido(a, b)) {
        puts("Periodo invalido.");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    int inicio = data_numero(a), fim = data_numero(b), total = 0, registros = 0;
    time_t agora = time(NULL);
    struct tm *data_atual = localtime(&agora);
    char hoje[11];
    if (!data_atual || !strftime(hoje, sizeof(hoje), "%d/%m/%Y", data_atual)) {
        puts("Nao foi possivel obter a data atual.");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
    int ultimo_dia = data_numero(hoje) + 1;
    double faturamento = 0;
    Reserva *r = obter_reservas();
    Chale *c = obter_chales();
    puts("Chale   | Dias ocupados | Ocupacao");
    for (int j = 0; j < obter_total_chales(); j++) {
        int n = 0;
        /* Uniao dos periodos evita contar duas vezes um dia com troca de hospede. */
        int proximo = inicio;
        while (proximo < fim) {
            int alcance = proximo;
            int inicio_mais_proximo = fim;
            for (int i = 0; i < obter_total_reservas(); i++) {
                if (!r[i].checkedIn || strcmp(r[i].chale, c[j].id) != 0) {
                    continue;
                }
                char data[11];
                memcpy(data, r[i].entrada_real, 10);
                data[10] = '\0';
                int entrada = data_numero(data);
                int saida = ultimo_dia;
                if (r[i].checkedOut) {
                    memcpy(data, r[i].saida_real, 10);
                    data[10] = '\0';
                    saida = data_numero(data);
                    if (saida == entrada) {
                        saida++;
                    }
                }
                if (entrada < 0 || saida <= proximo || entrada >= fim) {
                    continue;
                }
                if (entrada <= proximo) {
                    if (saida > alcance) {
                        alcance = saida;
                    }
                } else if (entrada < inicio_mais_proximo) {
                    inicio_mais_proximo = entrada;
                }
            }
            if (alcance > proximo) {
                int ate = alcance < fim ? alcance : fim;
                n += ate - proximo;
                proximo = ate;
                registros++;
            } else {
                proximo = inicio_mais_proximo;
            }
        }
        total += n;
        printf("Chale %-2d | %13d | %7.2f%%\n", j + 1, n, 100.0 * n / (fim - inicio));
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
    printf("Taxa geral (estadias): %.2f%% | Pagamentos recebidos: R$ %.2f\n",
           100.0 * total / ((fim - inicio) * obter_total_chales()), faturamento);
    pausar_sistema();
    return ESTADO_MENU_PRINCIPAL;
}

static EstadoSistema exibir_tela_usuarios(const SessaoUsuario *sessao) {
    if (!sessao || sessao->perfil != PERFIL_ADMIN) {
        return ESTADO_MENU_PRINCIPAL;
    }
    UsuarioResumo usuarios[MAX_USUARIOS];
    int total = listar_usuarios(usuarios, MAX_USUARIOS);
    LIMPAR_TELA();
    for (int i = 0; i < total; i++) {
        printf("[%d] %s | %s | Perfil %d\n", i + 1, usuarios[i].usuario,
               usuarios[i].nome, usuarios[i].perfil);
    }
    printf("[1] Cadastrar [2] Atualizar [3] Excluir [0] Voltar: ");
    int opcao = ler_opcao_segura();
    if (opcao < 1 || opcao > 3) {
        return ESTADO_MENU_PRINCIPAL;
    }
    char usuario[32], nome[50], senha[32];
    if (opcao == 1) {
        if (!linha("Usuario (sem espacos): ", usuario, sizeof(usuario)) ||
            strpbrk(usuario, " \t")) {
            puts("Usuario invalido.");
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
        }
        for (int i = 0; i < total; i++) {
            if (strcmp(usuario, usuarios[i].usuario) == 0) {
                puts("Usuario ja cadastrado.");
                pausar_sistema();
                return ESTADO_MENU_PRINCIPAL;
            }
        }
    } else {
        printf("Escolha o usuario pelo numero da lista: ");
        int escolhido = ler_opcao_segura();
        if (escolhido < 1 || escolhido > total) {
            return ESTADO_MENU_PRINCIPAL;
        }
        snprintf(usuario, sizeof(usuario), "%s", usuarios[escolhido - 1].usuario);
        if (strcmp(usuario, sessao->usuario) == 0) {
            puts("Nao e possivel alterar ou excluir a propria conta durante a sessao.");
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
        }
    }
    if (opcao == 3) {
        printf("Confirmar exclusao? [1] Sim: ");
        if (ler_opcao_segura() == 1 && excluir_usuario(sessao->usuario, usuario)) {
            adicionar_log_auditoria(sessao->usuario, "EXCLUI_USUARIO", usuario);
            puts("Usuario excluido.");
        } else {
            puts("Exclusao cancelada ou nao permitida.");
        }
    } else {
        if (!linha("Nome: ", nome, sizeof(nome))) {
            return ESTADO_MENU_PRINCIPAL;
        }
        printf("Perfil [1] Recepcao [2] Governanca [3] Administrador: ");
        int perfil = ler_opcao_segura();
        if (!linha("Senha (6 a 31 caracteres, sem espacos): ", senha, sizeof(senha)) ||
            strpbrk(senha, " \t")) {
            puts("Senha invalida.");
            pausar_sistema();
            return ESTADO_MENU_PRINCIPAL;
        }
        int salvo = salvar_usuario(usuario, nome, (PerfilUsuario)perfil, senha);
        memset(senha, 0, sizeof(senha));
        if (salvo) {
            adicionar_log_auditoria(sessao->usuario, "SALVA_USUARIO", usuario);
            puts("Usuario salvo nesta execucao.");
        } else {
            puts("Dados invalidos, limite atingido ou ultimo administrador.");
        }
    }
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
    printf(" [1] Cadastrar Hóspede\n");
    printf(" [2] Buscar Hóspede\n");
    printf(" [3] Gerenciar Hóspedes\n");
    printf(" [4] Consultar Disponibilidade & Nova Reserva\n");
    printf(" [5] Mapa de Ocupação Mensal\n");
    printf(" [6] Realizar Check-in\n");
    printf(" [7] Lançar Consumo / Extrato da Conta\n");
    printf(" [8] Realizar Check-out & Fechamento\n");
    printf(" [9] Alterar / Cancelar Reserva\n");
    printf(" [10] Governança / Manutenção de Chalés\n");
    if (sessao->perfil == PERFIL_ADMIN) {
        printf(" [11] Relatório de Auditoria de Operações\n");
        printf(" [12] Cadastro de Chalés\n");
        printf(" [13] Relatório de Ocupação e Faturamento\n");
        printf(" [14] Gerenciar Usuários\n");
    }
    printf(" [0] Sair (Logoff)\n");
    printf("\n--------------------------------------------------------------------------------\n");
    printf("Opção desejada > ");

    int opcao = ler_opcao_segura();

    switch (opcao) {
    case 1:
        return ESTADO_CADASTRAR_HOSPEDE;
    case 2:
        return ESTADO_BUSCAR_HOSPEDE;
    case 3:
        return ESTADO_HOSPEDE;
    case 4:
        return ESTADO_DISPONIBILIDADE;
    case 5:
        return ESTADO_MAPA_OCUPACAO;
    case 6:
        return ESTADO_CHECK_IN;
    case 7:
        return ESTADO_CONSUMO;
    case 8:
        return ESTADO_CHECK_OUT;
    case 9:
        return ESTADO_ALTERAR_CANCELAR;
    case 10:
        return ESTADO_GOVERNANCA;
    case 11:
        return sessao->perfil == PERFIL_ADMIN ? ESTADO_AUDITORIA : ESTADO_MENU_PRINCIPAL;
    case 12:
        return sessao->perfil == PERFIL_ADMIN ? ESTADO_CHALES : ESTADO_MENU_PRINCIPAL;
    case 13:
        return sessao->perfil == PERFIL_ADMIN ? ESTADO_RELATORIO : ESTADO_MENU_PRINCIPAL;
    case 14:
        return sessao->perfil == PERFIL_ADMIN ? ESTADO_USUARIOS : ESTADO_MENU_PRINCIPAL;
    case 0:
        return ESTADO_SAIR;
    default:
        printf("\nOpção inválida! Digite apenas números do menu.\n");
        pausar_sistema();
        return ESTADO_MENU_PRINCIPAL;
    }
}

void executar_painel_recepcao(const SessaoUsuario *sessao) {
    if (!sessao || (sessao->perfil != PERFIL_RECEPCAO && sessao->perfil != PERFIL_ADMIN)) {
        return;
    }
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
            estado = exibir_tela_mapa(sessao);
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
        case ESTADO_CADASTRAR_HOSPEDE:
            estado = operar_hospedes(sessao, 1);
            break;
        case ESTADO_BUSCAR_HOSPEDE:
            estado = operar_hospedes(sessao, 2);
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
        case ESTADO_USUARIOS:
            estado = exibir_tela_usuarios(sessao);
            break;
        case ESTADO_AUDITORIA:
            // Conexão com o relatório de auditoria desenvolvido por Rayane
            estado = sessao->perfil == PERFIL_ADMIN ? exibir_tela_auditoria(sessao)
                                                    : ESTADO_MENU_PRINCIPAL;
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
