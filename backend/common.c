#include "common.h"

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
