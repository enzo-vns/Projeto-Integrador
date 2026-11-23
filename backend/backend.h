#ifndef BACKEND_H
#define BACKEND_H

/* COMMON */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Comando de limpar tela dependendo do sistema operacional
#ifdef _WIN32
#define LIMPAR_TELA() system("cls")
#else
#define LIMPAR_TELA() system("clear")
#endif

typedef enum {
    ESTADO_SAIR = 0,
    ESTADO_LOGIN,
    ESTADO_MENU_PRINCIPAL,
    ESTADO_DISPONIBILIDADE,
    ESTADO_MAPA_OCUPACAO,
    ESTADO_CHECK_IN,
    ESTADO_CONSUMO,
    ESTADO_CHECK_OUT,
    ESTADO_ALTERAR_CANCELAR,
    ESTADO_HOSPEDE,
    ESTADO_CADASTRAR_HOSPEDE,
    ESTADO_BUSCAR_HOSPEDE,
    ESTADO_AUDITORIA,
    ESTADO_CHALES,
    ESTADO_RELATORIO,
    ESTADO_GOVERNANCA,
    ESTADO_USUARIOS
} EstadoSistema;

// Perfis de acesso dos usuários
typedef enum { PERFIL_NENHUM = 0, PERFIL_RECEPCAO, PERFIL_GOVERNANCA, PERFIL_ADMIN } PerfilUsuario;

// Informações da sessão autenticada
typedef struct {
    char usuario[32];
    char nome[50];
    PerfilUsuario perfil;
} SessaoUsuario;

// Utilitários de terminal e entrada de dados
void limpar_buffer(void);
int ler_opcao_segura(void);
void pausar_sistema(void);
void obter_data_hora_atual(char *buffer, size_t tamanho);

/* CHALES */
#define NUM_CHALES 4
#define DIAS_VISIVEIS 7
#define TOTAL_DIAS 14
#define MAX_CHALES 100

// Estados dos chalés (desenvolvido por Carlos)
typedef enum { LIVRE, RESERVADO, OCUPADO, LIMPEZA, MANUTENCAO } StatusChale;

// Estrutura do chalé com controle temporal de ocupação (Carlos)
typedef struct {
    char id[8];
    char tipo[16];
    int capacidade;
    double tarifa;
    StatusChale status[TOTAL_DIAS];
} Chale;

// Funções de inicialização e consulta
void inicializar_chales(void);
Chale *obter_chales(void);
int obter_total_chales(void);
int cadastrar_chale(const char *id, const char *tipo, int capacidade, double tarifa);
int contar_chales_livres_hoje(void);
int excluir_chale(const char *id);

// Rótulos visuais para tabela ASCII
const char *rotulo_status(StatusChale s);
const char *nome_status(StatusChale s);

// Atualização de status
void atualizar_status_chale(const char *id_chale, int dia_offset, StatusChale novo_status);
Chale *buscar_chale_por_id(const char *id_chale);

/* HOSPEDES */
#define MAX_HOSPEDES 50

typedef struct {
    int id;
    char nome[50];
    char documento[20];
    char telefone[20];
    char endereco[100];
    char email[80];
} Hospede;

void inicializar_hospedes(void);
Hospede *obter_hospedes(void);
int obter_total_hospedes(void);
int cadastrar_hospede(const char *nome, const char *documento, const char *telefone);
int documentos_iguais(const char *primeiro, const char *segundo);
Hospede *buscar_hospede_por_id(int id);
int excluir_hospede(int id);

/* RESERVAS */
#define MAX_RESERVAS 10000

typedef struct {
    int id, hospede_id;
    char hospede[50], chale[8], checkin[12], checkout[12], status[16];
    int checkedIn, checkedOut;
    char entrada_real[24], saida_real[24], motivo[60], pagamento[24];
    double tarifa, total_pago;
} Reserva;
void inicializar_reservas(void);
Reserva *obter_reservas(void);
int obter_total_reservas(void);
int contar_checkins_pendentes(void);
int contar_checkouts_pendentes(void);
int data_numero(const char *data);
int periodo_valido(const char *entrada, const char *saida);
int periodo_reserva_valido(const char *entrada, const char *saida);
int chale_disponivel(const char *chale, const char *entrada, const char *saida, int ignorar_id);
int criar_reserva(int hospede_id, const char *chale, const char *checkin, const char *checkout);
int alterar_reserva(int id, int hospede_id, const char *chale, const char *checkin,
                    const char *checkout);
int efetuar_checkin(int id_reserva);
int efetuar_checkout(int id_reserva);
int cancelar_reserva(int id_reserva);
Reserva *buscar_reserva_por_id(int id);
int excluir_reserva(int id);

/* CONSUMO */
#define MAX_CONSUMO 100

typedef struct {
    int id;
    int reserva_id;
    char chale[8];
    char item[50];
    float valor;
    char datahora[24];
} ItemConsumo;

void inicializar_consumo(void);
int lancar_consumo(int reserva_id, const char *item, float valor);
double total_consumo(int reserva_id);
ItemConsumo *obter_historico_consumo(void);
int obter_total_consumos(void);

/* AUDITORIA */
#define MAX_LOGS 100

// Estrutura de Log de Auditoria desenvolvida originalmente por Rayane
typedef struct {
    char datahora[24];
    char usuario[24];
    char acao[32];
    char detalhes[80];
} LogAuditoria;

// Inicializa a base de auditoria em memoria
void inicializar_auditoria(void);

// Registra uma nova operação crítica na auditoria
void adicionar_log_auditoria(const char *usuario, const char *acao, const char *detalhes);

// Retorna o ponteiro para o array de logs
LogAuditoria *obter_logs_auditoria(void);

// Retorna a quantidade total de registros gravados
int obter_total_logs_auditoria(void);

/* AUTH */
#define MAX_USUARIOS 50
typedef struct {
    char usuario[32];
    char nome[50];
    PerfilUsuario perfil;
} UsuarioResumo;
int listar_usuarios(UsuarioResumo *saida, int limite);
int salvar_usuario(const char *usuario, const char *nome, PerfilUsuario perfil, const char *senha);
int excluir_usuario(const char *usuario_atual, const char *usuario);

// Realiza a verificação de credenciais e retorna 1 se autenticado com sucesso
int autenticar_usuario(const char *usuario, const char *senha, SessaoUsuario *sessao);

#endif
