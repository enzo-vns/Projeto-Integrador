#include "backend.h"
#include <ctype.h>
#include <math.h>

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
    int opcao;
    int resultado = scanf("%d", &opcao);
    limpar_buffer();

    if (resultado != 1) {
        return -1;
    }
    return opcao;
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
        case LIVRE:      return "[ LIVR ]";
        case RESERVADO:  return "[ RESV ]";
        case OCUPADO:    return "[ OCUP ]";
        case LIMPEZA:    return "[ LIMP ]";
        case MANUTENCAO: return "[ MANU ]";
        default:         return "[ ???? ]";
    }
}

const char *nome_status(StatusChale s) {
    switch (s) {
        case LIVRE:      return "Livre";
        case RESERVADO:  return "Reservado";
        case OCUPADO:    return "Ocupado";
        case LIMPEZA:    return "Limpeza";
        case MANUTENCAO: return "Manutenção";
        default:         return "Desconhecido";
    }
}

void inicializar_chales(void) {
    if (g_inicializado_chales) return;

    for (int i = 0; i < NUM_CHALES; i++) {
        snprintf(g_chales[i].id, sizeof(g_chales[i].id), "CH-%02d", i + 1);
        strcpy(g_chales[i].tipo, "Standard");
        g_chales[i].capacidade = 2;
        g_chales[i].tarifa = 0.0; /* Informar tarifa real no cadastro administrativo. */
        for (int d = 0; d < TOTAL_DIAS; d++) g_chales[i].status[d] = LIVRE;
    }

    g_inicializado_chales = 1;
}

Chale* obter_chales(void) {
    inicializar_chales();
    return g_chales;
}

int obter_total_chales(void) {
    return g_total_chales;
}

int contar_chales_livres_hoje(void) {
    inicializar_chales();
    int livres = 0;
    for (int i = 0; i < g_total_chales; i++) {
        if (g_chales[i].status[0] == LIVRE) {
            livres++;
        }
    }
    return livres;
}

void atualizar_status_chale(const char *id_chale, int dia_offset, StatusChale novo_status) {
    inicializar_chales();
    for (int i = 0; i < g_total_chales; i++) {
        if (strcmp(g_chales[i].id, id_chale) == 0) {
            if (dia_offset >= 0 && dia_offset < TOTAL_DIAS) {
                g_chales[i].status[dia_offset] = novo_status;
            }
            break;
        }
    }
}

Chale* buscar_chale_por_id(const char *id_chale) {
    inicializar_chales();
    for (int i = 0; i < g_total_chales; i++) {
        if (strcmp(g_chales[i].id, id_chale) == 0) {
            return &g_chales[i];
        }
    }
    return NULL;
}

int cadastrar_chale(const char *id, const char *tipo, int capacidade, double tarifa) {
    inicializar_chales();
    if (!id || !*id || strlen(id)>=sizeof(g_chales[0].id) || !tipo || !*tipo || strlen(tipo)>=sizeof(g_chales[0].tipo) || capacidade<1 || tarifa<0 || g_total_chales>=MAX_CHALES || buscar_chale_por_id(id)) return 0;
    Chale *c=&g_chales[g_total_chales++]; memset(c,0,sizeof(*c));
    strcpy(c->id,id); strcpy(c->tipo,tipo); c->capacidade=capacidade; c->tarifa=tarifa; return 1;
}

/* HOSPEDES */
static Hospede g_hospedes[MAX_HOSPEDES];
static int g_total_hospedes = 0;
static int g_inicializado_hospedes = 0;

void inicializar_hospedes(void) {
    if (g_inicializado_hospedes) return;

    g_hospedes[0] = (Hospede){ 1, "Carlos Andrade", "321.654.987-00", "(11) 98888-1122", "", "" };
    g_hospedes[1] = (Hospede){ 2, "Beatriz Lima",   "112.334.556-70", "(11) 97777-3344", "", "" };
    g_hospedes[2] = (Hospede){ 3, "Renato Souza",   "998.877.665-40", "(11) 96666-5566", "", "" };
    g_total_hospedes = 3;

    g_inicializado_hospedes = 1;
}

Hospede* obter_hospedes(void) {
    inicializar_hospedes();
    return g_hospedes;
}

int obter_total_hospedes(void) {
    inicializar_hospedes();
    return g_total_hospedes;
}

int cadastrar_hospede(const char *nome, const char *documento, const char *telefone) {
    inicializar_hospedes();

    if (g_total_hospedes >= MAX_HOSPEDES) {
        return -1; // Limite atingido
    }

    Hospede novo = {0};
    if (!nome || !*nome || !documento || !*documento) return -1;
    for (int i=0; i<g_total_hospedes; i++) if (!strcmp(g_hospedes[i].documento, documento)) return -1;
    novo.id = g_total_hospedes + 1;
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

Hospede* buscar_hospede_por_id(int id) {
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
void inicializar_reservas(void) { }
Reserva *obter_reservas(void) { return g_reservas; }
int obter_total_reservas(void) { return g_total_reservas; }

/* Numero de dias em calendario gregoriano, sem depender de fuso horario. */
int data_numero(const char *s) {
    int d, m, y, dias, leap;
    static const int mes[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (!s || strlen(s) != 10 || s[2] != '/' || s[5] != '/') return -1;
    for (int i = 0; i < 10; i++) if (i != 2 && i != 5 && !isdigit((unsigned char)s[i])) return -1;
    d = (s[0]-'0')*10+s[1]-'0'; m = (s[3]-'0')*10+s[4]-'0';
    y = (s[6]-'0')*1000+(s[7]-'0')*100+(s[8]-'0')*10+s[9]-'0';
    if (y < 1900 || m < 1 || m > 12) return -1;
    leap = y%4 == 0 && (y%100 != 0 || y%400 == 0);
    if (d < 1 || d > mes[m] + (m == 2 && leap)) return -1;
    y--; dias = 365*y+y/4-y/100+y/400;
    for (int i=1; i<m; i++) dias += mes[i] + (i == 2 && leap);
    return dias+d;
}
int periodo_valido(const char *a, const char *b) { int x=data_numero(a), y=data_numero(b); return x >= 0 && y > x; }
int chale_disponivel(const char *id, const char *a, const char *b, int ignorar) {
    Chale *c = buscar_chale_por_id(id);
    if (!c || !periodo_valido(a,b) || c->status[0] == MANUTENCAO || c->status[0] == LIMPEZA) return 0;
    for (int i=0; i<g_total_reservas; i++) {
        Reserva *r=&g_reservas[i];
        if (r->id != ignorar && strcmp(r->chale,id)==0 && strcmp(r->status,"cancelada")!=0 &&
            data_numero(a)<data_numero(r->checkout) && data_numero(r->checkin)<data_numero(b)) return 0;
        if (r->id != ignorar && strcmp(r->chale,id)==0 && r->checkedIn && !r->checkedOut && data_numero(b)>data_numero(r->checkin)) return 0;
    }
    return 1;
}
int criar_reserva(int hospede_id, const char *id, const char *a, const char *b) {
    Hospede *h=buscar_hospede_por_id(hospede_id); Chale *c=buscar_chale_por_id(id);
    if (!h || !c || !periodo_valido(a,b)) return -1;
    if (!chale_disponivel(id,a,b,0)) return -2;
    if (g_total_reservas >= MAX_RESERVAS) return -3;
    Reserva *r=&g_reservas[g_total_reservas]; memset(r,0,sizeof(*r));
    r->id=++g_total_reservas; r->hospede_id=h->id; snprintf(r->hospede,sizeof(r->hospede),"%s",h->nome);
    snprintf(r->chale,sizeof(r->chale),"%s",id); snprintf(r->checkin,sizeof(r->checkin),"%s",a);
    snprintf(r->checkout,sizeof(r->checkout),"%s",b); strcpy(r->status,"confirmada"); r->tarifa=c->tarifa;
    return r->id;
}
Reserva *buscar_reserva_por_id(int id) { for(int i=0;i<g_total_reservas;i++) if(g_reservas[i].id==id) return &g_reservas[i]; return NULL; }
int alterar_reserva(int id,int hid,const char *c,const char *a,const char *b) {
    Reserva *r=buscar_reserva_por_id(id); Hospede *h=buscar_hospede_por_id(hid); Chale *ch=buscar_chale_por_id(c);
    if(!r || !h || !ch || r->checkedIn || strcmp(r->status,"confirmada") || !periodo_valido(a,b)) return -1;
    if(!chale_disponivel(c,a,b,id)) return -2;
    r->hospede_id=hid; snprintf(r->hospede,sizeof(r->hospede),"%s",h->nome); snprintf(r->chale,sizeof(r->chale),"%s",c);
    snprintf(r->checkin,sizeof(r->checkin),"%s",a); snprintf(r->checkout,sizeof(r->checkout),"%s",b); r->tarifa=ch->tarifa; return 1;
}
int efetuar_checkin(int id) {
    Reserva *r=buscar_reserva_por_id(id); Chale *c=r?buscar_chale_por_id(r->chale):NULL;
    if(!r || !c || r->checkedIn || strcmp(r->status,"confirmada") || c->status[0]==OCUPADO || c->status[0]==LIMPEZA || c->status[0]==MANUTENCAO) return 0;
    r->checkedIn=1; obter_data_hora_atual(r->entrada_real,sizeof(r->entrada_real)); atualizar_status_chale(r->chale,0,OCUPADO); return 1;
}
int efetuar_checkout(int id) { Reserva *r=buscar_reserva_por_id(id); if(!r || !r->checkedIn || r->checkedOut) return 0; r->checkedOut=1; obter_data_hora_atual(r->saida_real,sizeof(r->saida_real)); atualizar_status_chale(r->chale,0,LIMPEZA); return 1; }
int cancelar_reserva(int id) { Reserva *r=buscar_reserva_por_id(id); if(!r || r->checkedIn) return 0; if(!strcmp(r->status,"cancelada")) return -1; strcpy(r->status,"cancelada"); return 1; }
int contar_checkins_pendentes(void) { int n=0; for(int i=0;i<g_total_reservas;i++) if(!strcmp(g_reservas[i].status,"confirmada")&&!g_reservas[i].checkedIn) n++; return n; }
int contar_checkouts_pendentes(void) { int n=0; for(int i=0;i<g_total_reservas;i++) if(g_reservas[i].checkedIn&&!g_reservas[i].checkedOut) n++; return n; }

/* CONSUMO */
static ItemConsumo g_consumos[MAX_CONSUMO];
static int g_total_consumos = 0;
static int g_inicializado_consumo = 0;

void inicializar_consumo(void) {
    if (g_inicializado_consumo) return;
    g_total_consumos = 0;
    g_inicializado_consumo = 1;
}

int lancar_consumo(int reserva_id, const char *item, float valor) {
    Reserva *r = buscar_reserva_por_id(reserva_id);
    if (!r || !r->checkedIn || r->checkedOut || !item || !*item || !isfinite(valor) || valor <= 0) return -1;
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

ItemConsumo* obter_historico_consumo(void) {
    inicializar_consumo();
    return g_consumos;
}

int obter_total_consumos(void) {
    inicializar_consumo();
    return g_total_consumos;
}

double total_consumo(int id) { double total=0; for(int i=0;i<g_total_consumos;i++) if(g_consumos[i].reserva_id==id) total+=g_consumos[i].valor; return total; }

/* AUDITORIA */
static LogAuditoria g_logs[MAX_LOGS];
static int g_quantidade_logs = 0;
static int g_inicializado_auditoria = 0;

void inicializar_auditoria(void) {
    if (g_inicializado_auditoria) return;

    // Registros iniciais baseados nos dados originais da Rayane
    strcpy(g_logs[0].datahora, "20/09/2026 18:36");
    strcpy(g_logs[0].usuario,  "ana.gov");
    strcpy(g_logs[0].acao,     "STATUS_CHALE");
    strcpy(g_logs[0].detalhes, "CH-02 alterado para LIMPEZA");

    strcpy(g_logs[1].datahora, "20/09/2026 21:00");
    strcpy(g_logs[1].usuario,  "marcos.rec");
    strcpy(g_logs[1].acao,     "CHECKIN_REALIZADO");
    strcpy(g_logs[1].detalhes, "Reserva #87 (Hospede: Carlos)");

    strcpy(g_logs[2].datahora, "20/09/2026 21:43");
    strcpy(g_logs[2].usuario,  "marcos.rec");
    strcpy(g_logs[2].acao,     "CANCELA_RESERVA");
    strcpy(g_logs[2].detalhes, "Reserva #97 - Motivo: Desistencia");

    g_quantidade_logs = 3;
    g_inicializado_auditoria = 1;
}

void adicionar_log_auditoria(const char *usuario, const char *acao, const char *detalhes) {
    inicializar_auditoria();

    if (g_quantidade_logs < MAX_LOGS) {
        // Desloca os logs existentes para abrir a primeira posição para o registro mais recente
        for (int i = g_quantidade_logs; i > 0; i--) {
            g_logs[i] = g_logs[i - 1];
        }
        obter_data_hora_atual(g_logs[0].datahora, sizeof(g_logs[0].datahora));
        strncpy(g_logs[0].usuario, usuario ? usuario : "sistema", sizeof(g_logs[0].usuario) - 1);
        strncpy(g_logs[0].acao, acao ? acao : "OPERACAO", sizeof(g_logs[0].acao) - 1);
        strncpy(g_logs[0].detalhes, detalhes ? detalhes : "", sizeof(g_logs[0].detalhes) - 1);
        g_quantidade_logs++;
    }
}

LogAuditoria* obter_logs_auditoria(void) {
    inicializar_auditoria();
    return g_logs;
}

int obter_total_logs_auditoria(void) {
    inicializar_auditoria();
    return g_quantidade_logs;
}

/* AUTH */
int autenticar_usuario(const char *usuario, const char *senha, SessaoUsuario *sessao) {
    if (!usuario || !senha || !sessao) return 0;

    if (strcmp(usuario, "marcos.recepcao") == 0 && strcmp(senha, "123456") == 0) {
        strcpy(sessao->usuario, "marcos.rec");
        strcpy(sessao->nome, "Marcos (Recepcao)");
        sessao->perfil = PERFIL_RECEPCAO;
        adicionar_log_auditoria(sessao->usuario, "LOGIN", "Acesso ao painel da recepcao");
        return 1;
    }

    if (strcmp(usuario, "diretoria.admin") == 0 && (strcmp(senha, "123456") == 0 || strcmp(senha, "admin") == 0)) {
        strcpy(sessao->usuario, "diretoria");
        strcpy(sessao->nome, "Diretoria (Administrador)");
        sessao->perfil = PERFIL_ADMIN;
        adicionar_log_auditoria(sessao->usuario, "LOGIN", "Acesso ao painel administrativo");
        return 1;
    }

    if (strcmp(usuario, "ana.governanca") == 0 && strcmp(senha, "123456") == 0) {
        strcpy(sessao->usuario, "ana.gov");
        strcpy(sessao->nome, "Ana (Governanca)");
        sessao->perfil = PERFIL_GOVERNANCA;
        adicionar_log_auditoria(sessao->usuario, "LOGIN", "Acesso ao painel de governanca");
        return 1;
    }

    return 0;
}
