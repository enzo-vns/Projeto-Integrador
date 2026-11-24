#include "backend.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

#if defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#elif defined(__APPLE__)
#include <CommonCrypto/CommonKeyDerivation.h>
#include <CommonCrypto/CommonCryptoError.h>
#include <CommonCrypto/CommonRandom.h>
#else
#include <openssl/evp.h>
#include <openssl/rand.h>
#endif

/* COMMON */
// Limpa o buffer de entrada do teclado para evitar loop infinito
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // descarta dados residuais
    }
}

// Lê um número inteiro de forma segura
int ler_opcao_segura(void) {
    char entrada[64];
    char *fim;
    long valor;

    if (!fgets(entrada, sizeof(entrada), stdin)) {
        return -1;
    }
    if (!strchr(entrada, '\n') && !feof(stdin)) {
        limpar_buffer();
        return -1;
    }
    errno = 0;
    valor = strtol(entrada, &fim, 10);
    if (fim == entrada || errno == ERANGE || valor < INT_MIN || valor > INT_MAX) {
        return -1;
    }
    while (isspace((unsigned char)*fim)) {
        fim++;
    }
    return *fim == '\0' ? (int)valor : -1;
}

// Pausa a execução para leitura do usuário antes de limpar a tela
void pausar_sistema(void) {
    printf("\nPressione [ENTER] para continuar...");
    getchar();
}

// Obtém a data e hora formatada no padrão DD/MM/AAAA HH:MM
void obter_data_hora_atual(char *buffer, size_t tamanho) {
    time_t agora = time(NULL);
    struct tm *info = localtime(&agora);
    strftime(buffer, tamanho, "%d/%m/%Y %H:%M", info);
}

/* CHALES */
static Chale g_chales[MAX_CHALES];
static int g_total_chales = NUM_CHALES;
static int g_inicializado_chales = 0;

const char *rotulo_status(StatusChale s) {
    switch (s) {
    case LIVRE:
        return "[ LIVR ]";
    case RESERVADO:
        return "[ RESV ]";
    case OCUPADO:
        return "[ OCUP ]";
    case LIMPEZA:
        return "[ LIMP ]";
    case MANUTENCAO:
        return "[ MANU ]";
    default:
        return "[ ???? ]";
    }
}

const char *nome_status(StatusChale s) {
    switch (s) {
    case LIVRE:
        return "Livre";
    case RESERVADO:
        return "Reservado";
    case OCUPADO:
        return "Ocupado";
    case LIMPEZA:
        return "Limpeza";
    case MANUTENCAO:
        return "Manutenção";
    default:
        return "Desconhecido";
    }
}

void inicializar_chales(void) {
    if (g_inicializado_chales) {
        return;
    }

    /* Unidades e tarifas ficticias para exercitar o sistema. */
    static const char *tipos[NUM_CHALES] = {
        "Standard", "Standard", "Standard", "Standard", "Familia", "Familia", "Luxo", "Luxo"
    };
    static const int capacidades[NUM_CHALES] = {2, 2, 2, 2, 4, 5, 2, 3};
    static const double tarifas[NUM_CHALES] = {150, 150, 170, 170, 220, 260, 300, 350};
    for (int i = 0; i < NUM_CHALES; i++) {
        snprintf(g_chales[i].id, sizeof(g_chales[i].id), "CH-%02d", i + 1);
        snprintf(g_chales[i].tipo, sizeof(g_chales[i].tipo), "%s", tipos[i]);
        g_chales[i].capacidade = capacidades[i];
        g_chales[i].tarifa = tarifas[i];
        for (int d = 0; d < TOTAL_DIAS; d++) {
            g_chales[i].status[d] = LIVRE;
        }
    }

    g_inicializado_chales = 1;
}

Chale *obter_chales(void) {
    inicializar_chales();
    return g_chales;
}

int obter_total_chales(void) {
    return g_total_chales;
}

int contar_chales_livres_hoje(void) {
    inicializar_chales();
    time_t agora = time(NULL);
    struct tm *data = localtime(&agora);
    char hoje[12];
    if (!data || !strftime(hoje, sizeof(hoje), "%d/%m/%Y", data)) {
        return 0;
    }
    int dia = data_numero(hoje);
    int livres = 0;
    Reserva *reservas = obter_reservas();
    for (int i = 0; i < g_total_chales; i++) {
        int disponivel = g_chales[i].status[0] == LIVRE;
        for (int j = 0; disponivel && j < obter_total_reservas(); j++) {
            Reserva *r = &reservas[j];
            if (strcmp(r->chale, g_chales[i].id) == 0 && strcmp(r->status, "cancelada") != 0 &&
                !r->checkedOut && dia >= data_numero(r->checkin) &&
                dia < data_numero(r->checkout)) {
                disponivel = 0;
            }
        }
        livres += disponivel;
    }
    return livres;
}

void atualizar_status_chale(const char *id_chale, int dia_offset, StatusChale novo_status) {
    inicializar_chales();
    if (!id_chale) {
        return;
    }
    for (int i = 0; i < g_total_chales; i++) {
        if (strcmp(g_chales[i].id, id_chale) == 0) {
            if (dia_offset >= 0 && dia_offset < TOTAL_DIAS) {
                g_chales[i].status[dia_offset] = novo_status;
            }
            break;
        }
    }
}

Chale *buscar_chale_por_id(const char *id_chale) {
    inicializar_chales();
    if (!id_chale) {
        return NULL;
    }
    for (int i = 0; i < g_total_chales; i++) {
        if (strcmp(g_chales[i].id, id_chale) == 0) {
            return &g_chales[i];
        }
    }
    return NULL;
}

int cadastrar_chale(const char *id, const char *tipo, int capacidade, double tarifa) {
    inicializar_chales();
    if (!isfinite(tarifa) || !id || !*id || strlen(id) >= sizeof(g_chales[0].id) || !tipo ||
        !*tipo || strlen(tipo) >= sizeof(g_chales[0].tipo) ||
        (strcmp(tipo, "Standard") != 0 && strcmp(tipo, "Familia") != 0 &&
         strcmp(tipo, "Luxo") != 0) || capacidade < 1 || tarifa < 0 ||
        g_total_chales >= MAX_CHALES || buscar_chale_por_id(id)) {
        return 0;
    }
    Chale *c = &g_chales[g_total_chales++];
    memset(c, 0, sizeof(*c));
    strcpy(c->id, id);
    strcpy(c->tipo, tipo);
    c->capacidade = capacidade;
    c->tarifa = tarifa;
    return 1;
}

/* HOSPEDES */
static Hospede g_hospedes[MAX_HOSPEDES];
static int g_total_hospedes = 0;
static int g_inicializado_hospedes = 0;

void inicializar_hospedes(void) {
    if (g_inicializado_hospedes) {
        return;
    }

    g_hospedes[0] = (Hospede){1, "Carlos Andrade", "321.654.987-00", "(11) 98888-1122", "", ""};
    g_hospedes[1] = (Hospede){2, "Beatriz Lima", "112.334.556-70", "(11) 97777-3344", "", ""};
    g_hospedes[2] = (Hospede){3, "Renato Souza", "998.877.665-40", "(11) 96666-5566", "", ""};
    /* Documentos ficticios identificados como dados de teste. */
    g_hospedes[3] = (Hospede){4, "Ana Ribeiro", "TESTE-004", "", "", ""};
    g_hospedes[4] = (Hospede){5, "Mariana Silva", "TESTE-005", "", "", ""};
    g_hospedes[5] = (Hospede){6, "Pedro Almeida", "TESTE-006", "", "", ""};
    g_hospedes[6] = (Hospede){7, "Julia Santos", "TESTE-007", "", "", ""};
    g_hospedes[7] = (Hospede){8, "Lucas Ferreira", "TESTE-008", "", "", ""};
    g_hospedes[8] = (Hospede){9, "Camila Oliveira", "TESTE-009", "", "", ""};
    g_hospedes[9] = (Hospede){10, "Rafael Costa", "TESTE-010", "", "", ""};
    g_total_hospedes = 10;

    g_inicializado_hospedes = 1;
}

Hospede *obter_hospedes(void) {
    inicializar_hospedes();
    return g_hospedes;
}

int obter_total_hospedes(void) {
    inicializar_hospedes();
    return g_total_hospedes;
}

/* Compara CPFs com e sem pontuacao; outros documentos usam o texto integral. */
static int extrair_cpf(const char *documento, char digitos[12]) {
    int quantidade = 0;
    for (const char *p = documento; *p; p++) {
        if (*p >= '0' && *p <= '9') {
            if (quantidade >= 11) {
                return 0;
            }
            digitos[quantidade++] = *p;
        } else if (*p != '.' && *p != '-') {
            return 0;
        }
    }
    digitos[quantidade] = '\0';
    return quantidade == 11;
}

int documentos_iguais(const char *primeiro, const char *segundo) {
    if (!primeiro || !segundo) {
        return 0;
    }
    char cpf_a[12], cpf_b[12];
    if (extrair_cpf(primeiro, cpf_a) && extrair_cpf(segundo, cpf_b)) {
        return strcmp(cpf_a, cpf_b) == 0;
    }
    return strcmp(primeiro, segundo) == 0;
}

int cadastrar_hospede(const char *nome, const char *documento, const char *telefone) {
    inicializar_hospedes();

    if (g_total_hospedes >= MAX_HOSPEDES) {
        return -1; // Limite atingido
    }

    Hospede novo = {0};
    if (!nome || !*nome || !documento || !*documento || !telefone) {
        return -1;
    }
    for (int i = 0; i < g_total_hospedes; i++) {
        if (documentos_iguais(g_hospedes[i].documento, documento)) {
            return -1;
        }
    }
    novo.id = 1;
    for (int i = 0; i < g_total_hospedes; i++) {
        if (g_hospedes[i].id >= novo.id) {
            novo.id = g_hospedes[i].id + 1;
        }
    }
    strncpy(novo.nome, nome, sizeof(novo.nome) - 1);
    novo.nome[sizeof(novo.nome) - 1] = '\0';

    strncpy(novo.documento, documento, sizeof(novo.documento) - 1);
    novo.documento[sizeof(novo.documento) - 1] = '\0';

    strncpy(novo.telefone, telefone, sizeof(novo.telefone) - 1);
    novo.telefone[sizeof(novo.telefone) - 1] = '\0';

    g_hospedes[g_total_hospedes] = novo;
    g_total_hospedes++;

    return novo.id;
}

Hospede *buscar_hospede_por_id(int id) {
    inicializar_hospedes();
    for (int i = 0; i < g_total_hospedes; i++) {
        if (g_hospedes[i].id == id) {
            return &g_hospedes[i];
        }
    }
    return NULL;
}

/* RESERVAS */
static Reserva g_reservas[MAX_RESERVAS];
static int g_total_reservas;
void inicializar_reservas(void) {
}
Reserva *obter_reservas(void) {
    return g_reservas;
}
int obter_total_reservas(void) {
    return g_total_reservas;
}

/* Numero de dias em calendario gregoriano, sem depender de fuso horario. */
int data_numero(const char *s) {
    int d, m, y, dias, leap;
    static const int mes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (!s || strlen(s) != 10 || s[2] != '/' || s[5] != '/') {
        return -1;
    }
    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && !isdigit((unsigned char)s[i])) {
            return -1;
        }
    }
    d = (s[0] - '0') * 10 + s[1] - '0';
    m = (s[3] - '0') * 10 + s[4] - '0';
    y = (s[6] - '0') * 1000 + (s[7] - '0') * 100 + (s[8] - '0') * 10 + s[9] - '0';
    if (y < 1900 || m < 1 || m > 12) {
        return -1;
    }
    leap = y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
    if (d < 1 || d > mes[m] + (m == 2 && leap)) {
        return -1;
    }
    y--;
    dias = 365 * y + y / 4 - y / 100 + y / 400;
    for (int i = 1; i < m; i++) {
        dias += mes[i] + (i == 2 && leap);
    }
    return dias + d;
}
int periodo_valido(const char *a, const char *b) {
    int x = data_numero(a), y = data_numero(b);
    return x >= 0 && y > x;
}
int periodo_reserva_valido(const char *entrada, const char *saida) {
    time_t agora = time(NULL);
    struct tm *hoje = localtime(&agora);
    char data_atual[12];
    if (!hoje || !strftime(data_atual, sizeof(data_atual), "%d/%m/%Y", hoje)) {
        return 0;
    }
    return periodo_valido(entrada, saida) && data_numero(entrada) >= data_numero(data_atual);
}
int chale_disponivel(const char *id, const char *a, const char *b, int ignorar) {
    Chale *c = buscar_chale_por_id(id);
    if (!c || !periodo_reserva_valido(a, b) || c->status[0] == MANUTENCAO ||
        c->status[0] == LIMPEZA) {
        return 0;
    }
    for (int i = 0; i < g_total_reservas; i++) {
        Reserva *r = &g_reservas[i];
        if (r->id != ignorar && strcmp(r->chale, id) == 0 && strcmp(r->status, "cancelada") != 0 &&
            data_numero(a) < data_numero(r->checkout) && data_numero(r->checkin) < data_numero(b)) {
            return 0;
        }
        if (r->id != ignorar && strcmp(r->chale, id) == 0 && r->checkedIn && !r->checkedOut &&
            data_numero(b) > data_numero(r->checkin)) {
            return 0;
        }
    }
    return 1;
}
int criar_reserva(int hospede_id, const char *id, const char *a, const char *b) {
    Hospede *h = buscar_hospede_por_id(hospede_id);
    Chale *c = buscar_chale_por_id(id);
    if (!h || !c || !periodo_reserva_valido(a, b)) {
        return -1;
    }
    if (!chale_disponivel(id, a, b, 0)) {
        return -2;
    }
    if (g_total_reservas >= MAX_RESERVAS) {
        return -3;
    }
    Reserva *r = &g_reservas[g_total_reservas];
    memset(r, 0, sizeof(*r));
    r->id = 1;
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].id >= r->id) {
            r->id = g_reservas[i].id + 1;
        }
    }
    g_total_reservas++;
    r->hospede_id = h->id;
    snprintf(r->hospede, sizeof(r->hospede), "%s", h->nome);
    snprintf(r->chale, sizeof(r->chale), "%s", id);
    snprintf(r->checkin, sizeof(r->checkin), "%s", a);
    snprintf(r->checkout, sizeof(r->checkout), "%s", b);
    strcpy(r->status, "confirmada");
    r->tarifa = c->tarifa;
    return r->id;
}
Reserva *buscar_reserva_por_id(int id) {
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].id == id) {
            return &g_reservas[i];
        }
    }
    return NULL;
}
/* Reservas ficticias relativas ao dia de abertura para demonstracao. */
static int data_exemplo(int deslocamento, char resultado[12]) {
    time_t agora = time(NULL);
    struct tm *atual = localtime(&agora);
    if (!atual) {
        return 0;
    }
    struct tm data = *atual;
    data.tm_hour = 12;
    data.tm_min = 0;
    data.tm_sec = 0;
    data.tm_mday += deslocamento;
    data.tm_isdst = -1;
    if (mktime(&data) == (time_t)-1 || !strftime(resultado, 12, "%d/%m/%Y", &data)) {
        return 0;
    }
    return 1;
}

int carregar_reservas_exemplo(void) {
    if (obter_total_reservas() != 0) {
        return 0;
    }
    static const int entradas[] = {0, 0, 0, 2, 4, 7, 10, 14, 20, 28, 36, 44};
    static const int saidas[] =   {3, 4, 2, 5, 7, 10, 13, 17, 23, 31, 39, 47};
    static const char *chales[] = {
        "CH-01", "CH-02", "CH-03", "CH-04", "CH-05", "CH-06",
        "CH-07", "CH-08", "CH-03", "CH-04", "CH-05", "CH-07"
    };
    int criadas = 0;
    for (size_t i = 0; i < sizeof(entradas) / sizeof(entradas[0]); i++) {
        char entrada[12], saida[12];
        if (!data_exemplo(entradas[i], entrada) || !data_exemplo(saidas[i], saida)) {
            break;
        }
        int reserva = criar_reserva((int)(i % 10) + 1, chales[i], entrada, saida);
        if (reserva > 0) {
            criadas++;
            if (i < 2 && efetuar_checkin(reserva)) {
                lancar_consumo(reserva, i == 0 ? "Cafe da manha (teste)" : "Agua (teste)",
                               i == 0 ? 35.0f : 8.0f);
            }
        }
    }
    return criadas;
}

int alterar_reserva(int id, int hid, const char *c, const char *a, const char *b) {
    Reserva *r = buscar_reserva_por_id(id);
    Hospede *h = buscar_hospede_por_id(hid);
    Chale *ch = buscar_chale_por_id(c);
    if (!r || !h || !ch || r->checkedIn || strcmp(r->status, "confirmada") ||
        !periodo_reserva_valido(a, b)) {
        return -1;
    }
    if (!chale_disponivel(c, a, b, id)) {
        return -2;
    }
    r->hospede_id = hid;
    snprintf(r->hospede, sizeof(r->hospede), "%s", h->nome);
    snprintf(r->chale, sizeof(r->chale), "%s", c);
    snprintf(r->checkin, sizeof(r->checkin), "%s", a);
    snprintf(r->checkout, sizeof(r->checkout), "%s", b);
    r->tarifa = ch->tarifa;
    return 1;
}
int efetuar_checkin(int id) {
    Reserva *r = buscar_reserva_por_id(id);
    Chale *c = r ? buscar_chale_por_id(r->chale) : NULL;
    if (!r || !c || r->checkedIn || strcmp(r->status, "confirmada") || c->status[0] == OCUPADO ||
        c->status[0] == LIMPEZA || c->status[0] == MANUTENCAO) {
        return 0;
    }
    time_t agora = time(NULL);
    struct tm *hoje = localtime(&agora);
    char data_atual[12];
    if (!hoje || !strftime(data_atual, sizeof(data_atual), "%d/%m/%Y", hoje) ||
        data_numero(data_atual) < data_numero(r->checkin) ||
        data_numero(data_atual) >= data_numero(r->checkout)) {
        return 0;
    }
    r->checkedIn = 1;
    obter_data_hora_atual(r->entrada_real, sizeof(r->entrada_real));
    atualizar_status_chale(r->chale, 0, OCUPADO);
    return 1;
}
int efetuar_checkout(int id) {
    Reserva *r = buscar_reserva_por_id(id);
    if (!r || !r->checkedIn || r->checkedOut) {
        return 0;
    }
    r->checkedOut = 1;
    obter_data_hora_atual(r->saida_real, sizeof(r->saida_real));
    atualizar_status_chale(r->chale, 0, LIMPEZA);
    return 1;
}
int cancelar_reserva(int id) {
    Reserva *r = buscar_reserva_por_id(id);
    if (!r || r->checkedIn) {
        return 0;
    }
    if (!strcmp(r->status, "cancelada")) {
        return -1;
    }
    strcpy(r->status, "cancelada");
    return 1;
}
int contar_checkins_pendentes(void) {
    int n = 0;
    for (int i = 0; i < g_total_reservas; i++) {
        if (!strcmp(g_reservas[i].status, "confirmada") && !g_reservas[i].checkedIn) {
            n++;
        }
    }
    return n;
}
int contar_checkouts_pendentes(void) {
    int n = 0;
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].checkedIn && !g_reservas[i].checkedOut) {
            n++;
        }
    }
    return n;
}

/* CONSUMO */
static ItemConsumo g_consumos[MAX_CONSUMO];
static int g_total_consumos = 0;
static int g_inicializado_consumo = 0;

void inicializar_consumo(void) {
    if (g_inicializado_consumo) {
        return;
    }
    g_total_consumos = 0;
    g_inicializado_consumo = 1;
}

int lancar_consumo(int reserva_id, const char *item, float valor) {
    Reserva *r = buscar_reserva_por_id(reserva_id);
    if (!r || !r->checkedIn || r->checkedOut || !item || !*item || !isfinite(valor) || valor <= 0) {
        return -1;
    }
    inicializar_consumo();

    if (g_total_consumos >= MAX_CONSUMO) {
        return -1;
    }

    ItemConsumo novo;
    novo.id = g_total_consumos + 1;
    novo.reserva_id = reserva_id;
    strncpy(novo.chale, r->chale, sizeof(novo.chale) - 1);
    novo.chale[sizeof(novo.chale) - 1] = '\0';

    strncpy(novo.item, item, sizeof(novo.item) - 1);
    novo.item[sizeof(novo.item) - 1] = '\0';

    novo.valor = valor;
    obter_data_hora_atual(novo.datahora, sizeof(novo.datahora));

    g_consumos[g_total_consumos++] = novo;
    return novo.id;
}

ItemConsumo *obter_historico_consumo(void) {
    inicializar_consumo();
    return g_consumos;
}

int obter_total_consumos(void) {
    inicializar_consumo();
    return g_total_consumos;
}

double total_consumo(int id) {
    double total = 0;
    for (int i = 0; i < g_total_consumos; i++) {
        if (g_consumos[i].reserva_id == id) {
            total += g_consumos[i].valor;
        }
    }
    return total;
}

/* AUDITORIA */
static LogAuditoria g_logs[MAX_LOGS];
static int g_quantidade_logs = 0;
static int g_inicializado_auditoria = 0;

void inicializar_auditoria(void) {
    if (g_inicializado_auditoria) {
        return;
    }

    g_quantidade_logs = 0;
    g_inicializado_auditoria = 1;
}

void adicionar_log_auditoria(const char *usuario, const char *acao, const char *detalhes) {
    inicializar_auditoria();

    int ultimo = g_quantidade_logs < MAX_LOGS ? g_quantidade_logs : MAX_LOGS - 1;
    for (int i = ultimo; i > 0; i--) {
        g_logs[i] = g_logs[i - 1];
    }
    obter_data_hora_atual(g_logs[0].datahora, sizeof(g_logs[0].datahora));
    snprintf(g_logs[0].usuario, sizeof(g_logs[0].usuario), "%s", usuario ? usuario : "sistema");
    snprintf(g_logs[0].acao, sizeof(g_logs[0].acao), "%s", acao ? acao : "OPERACAO");
    snprintf(g_logs[0].detalhes, sizeof(g_logs[0].detalhes), "%s", detalhes ? detalhes : "");
    if (g_quantidade_logs < MAX_LOGS) {
        g_quantidade_logs++;
    }
}

LogAuditoria *obter_logs_auditoria(void) {
    inicializar_auditoria();
    return g_logs;
}

int obter_total_logs_auditoria(void) {
    inicializar_auditoria();
    return g_quantidade_logs;
}

/* Exclusoes preservam referencias historicas e reservas em andamento. */
int excluir_chale(const char *id) {
    if (!id || g_total_chales <= 1) {
        return 0;
    }
    for (int i = 0; i < g_total_reservas; i++) {
        if (strcmp(g_reservas[i].chale, id) == 0) {
            return 0;
        }
    }
    for (int i = 0; i < g_total_chales; i++) {
        if (strcmp(g_chales[i].id, id) == 0 && g_chales[i].status[0] != OCUPADO) {
            memmove(&g_chales[i], &g_chales[i + 1],
                    (size_t)(g_total_chales - i - 1) * sizeof(g_chales[0]));
            memset(&g_chales[--g_total_chales], 0, sizeof(g_chales[0]));
            return 1;
        }
    }
    return 0;
}

int excluir_hospede(int id) {
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].hospede_id == id) {
            return 0;
        }
    }
    for (int i = 0; i < g_total_hospedes; i++) {
        if (g_hospedes[i].id == id) {
            memmove(&g_hospedes[i], &g_hospedes[i + 1],
                    (size_t)(g_total_hospedes - i - 1) * sizeof(g_hospedes[0]));
            memset(&g_hospedes[--g_total_hospedes], 0, sizeof(g_hospedes[0]));
            return 1;
        }
    }
    return 0;
}

int excluir_reserva(int id) {
    for (int i = 0; i < g_total_reservas; i++) {
        if (g_reservas[i].id == id && !g_reservas[i].checkedIn &&
            strcmp(g_reservas[i].status, "cancelada") == 0) {
            memmove(&g_reservas[i], &g_reservas[i + 1],
                    (size_t)(g_total_reservas - i - 1) * sizeof(g_reservas[0]));
            memset(&g_reservas[--g_total_reservas], 0, sizeof(g_reservas[0]));
            return 1;
        }
    }
    return 0;
}

/* AUTH: hashes PBKDF2-HMAC-SHA256 para as contas de demonstracao. */
typedef struct {
    char usuario[32];
    char nome[50];
    PerfilUsuario perfil;
    char salt[33];
    char hash[65];
} CredencialInicial;

static CredencialInicial contas[MAX_USUARIOS] = {
        {"marcos.recepcao", "Marcos (Recepcao)", PERFIL_RECEPCAO,
         "dcf7a631b89092f42bb74511ca92c613", "1fab4dd8463f5fff848745d5f208d64bb95de5911d6de8a5483d326bb4e1a51f"},
        {"diretoria.admin", "Diretoria (Administrador)", PERFIL_ADMIN,
         "bf404153a71f8d7019e83ca48ffef246", "89d2e613e49c3e17333a196349832f988b4a2f154ddbe249e64f56d75f834f07"},
        {"ana.governanca", "Ana (Governanca)", PERFIL_GOVERNANCA,
         "5e2362fb8d0a54e73b91f496d24a3981", "3855238fb4f56664580a3944d1db570436fff8d0c86d9cb9278559977da59fb1"}
};
static int total_contas = 3;

static int converter_hex(const char *texto, unsigned char *saida, size_t tamanho) {
    for (size_t i = 0; i < tamanho; i++) {
        unsigned int valor;
        if (sscanf(texto + i * 2, "%2x", &valor) != 1) {
            return 0;
        }
        saida[i] = (unsigned char)valor;
    }
    return 1;
}

#ifdef _WIN32
/* Usa o provedor criptografico do Windows para PBKDF2-HMAC-SHA256. */
static int derivar_senha_windows(const char *senha, unsigned char *salt, size_t tamanho_salt,
                                 unsigned char *saida, size_t tamanho_saida) {
    BCRYPT_ALG_HANDLE algoritmo = NULL;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&algoritmo, BCRYPT_SHA256_ALGORITHM, NULL,
                                                   BCRYPT_ALG_HANDLE_HMAC_FLAG);
    if (status < 0) {
        return 0;
    }
    status = BCryptDeriveKeyPBKDF2(algoritmo, (PUCHAR)senha, (ULONG)strlen(senha), salt,
                                   (ULONG)tamanho_salt, 310000, saida, (ULONG)tamanho_saida, 0);
    BCryptCloseAlgorithmProvider(algoritmo, 0);
    return status >= 0;
}
#endif

static int conferir_senha(const char *senha, const char *salt_hex, const char *hash_hex) {
    unsigned char salt[16], esperado[32], calculado[32];
    unsigned char diferenca = 0;
    if (!converter_hex(salt_hex, salt, sizeof(salt)) ||
        !converter_hex(hash_hex, esperado, sizeof(esperado))) {
        return 0;
    }
#if defined(_WIN32)
    if (!derivar_senha_windows(senha, salt, sizeof(salt), calculado, sizeof(calculado))) {
        return 0;
    }
#elif defined(__APPLE__)
    if (CCKeyDerivationPBKDF(kCCPBKDF2, senha, strlen(senha), salt, sizeof(salt),
                             kCCPRFHmacAlgSHA256, 310000, calculado, sizeof(calculado)) != kCCSuccess) {
        return 0;
    }
#else
    if (PKCS5_PBKDF2_HMAC(senha, (int)strlen(senha), salt, sizeof(salt), 310000,
                          EVP_sha256(), sizeof(calculado), calculado) != 1) {
        return 0;
    }
#endif
    for (size_t i = 0; i < sizeof(calculado); i++) {
        diferenca |= calculado[i] ^ esperado[i];
    }
    memset(calculado, 0, sizeof(calculado));
    return diferenca == 0;
}

int listar_usuarios(UsuarioResumo *saida, int limite) {
    if (!saida || limite < 0) {
        return 0;
    }
    int quantidade = total_contas < limite ? total_contas : limite;
    for (int i = 0; i < quantidade; i++) {
        snprintf(saida[i].usuario, sizeof(saida[i].usuario), "%s", contas[i].usuario);
        snprintf(saida[i].nome, sizeof(saida[i].nome), "%s", contas[i].nome);
        saida[i].perfil = contas[i].perfil;
    }
    return quantidade;
}

int salvar_usuario(const char *usuario, const char *nome, PerfilUsuario perfil, const char *senha) {
    unsigned char salt[16], hash[32];
    if (!usuario || !*usuario || strlen(usuario) >= sizeof(contas[0].usuario) ||
        !nome || !*nome || strlen(nome) >= sizeof(contas[0].nome) ||
        !senha || strlen(senha) < 6 || strlen(senha) > 31 ||
        perfil < PERFIL_RECEPCAO || perfil > PERFIL_ADMIN) {
        return 0;
    }
    int indice = total_contas;
    int administradores = 0;
    for (int i = 0; i < total_contas; i++) {
        if (strcmp(contas[i].usuario, usuario) == 0) {
            indice = i;
        }
        if (contas[i].perfil == PERFIL_ADMIN) {
            administradores++;
        }
    }
    if (indice == MAX_USUARIOS || (indice < total_contas && contas[indice].perfil == PERFIL_ADMIN &&
                                  perfil != PERFIL_ADMIN && administradores == 1)) {
        return 0;
    }
#if defined(_WIN32)
    if (BCryptGenRandom(NULL, salt, sizeof(salt), BCRYPT_USE_SYSTEM_PREFERRED_RNG) < 0 ||
        !derivar_senha_windows(senha, salt, sizeof(salt), hash, sizeof(hash))) {
        return 0;
    }
#elif defined(__APPLE__)
    if (CCRandomGenerateBytes(salt, sizeof(salt)) != kCCSuccess ||
        CCKeyDerivationPBKDF(kCCPBKDF2, senha, strlen(senha), salt, sizeof(salt),
                             kCCPRFHmacAlgSHA256, 310000, hash, sizeof(hash)) != kCCSuccess) {
        return 0;
    }
#else
    if (RAND_bytes(salt, sizeof(salt)) != 1 ||
        PKCS5_PBKDF2_HMAC(senha, (int)strlen(senha), salt, sizeof(salt), 310000,
                          EVP_sha256(), sizeof(hash), hash) != 1) {
        return 0;
    }
#endif
    CredencialInicial *c = &contas[indice];
    snprintf(c->usuario, sizeof(c->usuario), "%s", usuario);
    snprintf(c->nome, sizeof(c->nome), "%s", nome);
    c->perfil = perfil;
    for (size_t i = 0; i < sizeof(salt); i++) {
        snprintf(c->salt + i * 2, 3, "%02x", salt[i]);
    }
    for (size_t i = 0; i < sizeof(hash); i++) {
        snprintf(c->hash + i * 2, 3, "%02x", hash[i]);
    }
    memset(hash, 0, sizeof(hash));
    if (indice == total_contas) {
        total_contas++;
    }
    return 1;
}

int excluir_usuario(const char *usuario_atual, const char *usuario) {
    if (!usuario_atual || !usuario || strcmp(usuario_atual, usuario) == 0) {
        return 0;
    }
    int administradores = 0;
    int indice = -1;
    for (int i = 0; i < total_contas; i++) {
        if (contas[i].perfil == PERFIL_ADMIN) {
            administradores++;
        }
        if (strcmp(contas[i].usuario, usuario) == 0) {
            indice = i;
        }
    }
    if (indice < 0 || (contas[indice].perfil == PERFIL_ADMIN && administradores == 1)) {
        return 0;
    }
    memmove(&contas[indice], &contas[indice + 1],
            (size_t)(total_contas - indice - 1) * sizeof(contas[0]));
    memset(&contas[--total_contas], 0, sizeof(contas[0]));
    return 1;
}

int autenticar_usuario(const char *usuario, const char *senha, SessaoUsuario *sessao) {
    if (!sessao) {
        return 0;
    }
    memset(sessao, 0, sizeof(*sessao));
    if (!usuario || !senha) {
        return 0;
    }
    for (size_t i = 0; i < (size_t)total_contas; i++) {
        if (strcmp(usuario, contas[i].usuario) == 0 &&
            conferir_senha(senha, contas[i].salt, contas[i].hash)) {
            snprintf(sessao->usuario, sizeof(sessao->usuario), "%s", contas[i].usuario);
            snprintf(sessao->nome, sizeof(sessao->nome), "%s", contas[i].nome);
            sessao->perfil = contas[i].perfil;
            adicionar_log_auditoria(sessao->usuario, "LOGIN", "Acesso autenticado");
            return 1;
        }
    }
    return 0;
}
