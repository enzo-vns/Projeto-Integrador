#ifndef COMMON_H
#define COMMON_H

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

// Estados da máquina de estados (navegação entre as telas do sistema)
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
    ESTADO_AUDITORIA,
    ESTADO_GOVERNANCA
} EstadoSistema;

// Perfis de acesso dos usuários
typedef enum {
    PERFIL_NENHUM = 0,
    PERFIL_RECEPCAO,
    PERFIL_GOVERNANCA,
    PERFIL_ADMIN
} PerfilUsuario;

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

#endif // COMMON_H
