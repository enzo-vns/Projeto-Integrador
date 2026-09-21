#include "common.h"

// Limpa o buffer do teclado para evitar loop infinito
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // descarta caracteres excedentes
    }
}

// Lê número inteiro com validação de entrada
int ler_opcao_segura(void) {
    int opcao;
    int resultado = scanf("%d", &opcao);
    limpar_buffer();
    
    if (resultado != 1) {
        return -1;
    }
    return opcao;
}

// Pausa a tela para permitir leitura
void pausar_sistema(void) {
    printf("\nPressione [ENTER] para continuar...");
    getchar();
}

// Gera string formatada no padrão DD/MM/AAAA HH:MM
void obter_data_hora_atual(char *buffer, size_t tamanho) {
    time_t agora = time(NULL);
    struct tm *info = localtime(&agora);
    strftime(buffer, tamanho, "%d/%m/%Y %H:%M", info);
}
