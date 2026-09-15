#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char usuario[30];
    char senha[20];

    printf("=================\nRECANTO DO BOSQUE\n=================");
    printf("\nAutenticacao de funcionario");

    printf("\n\nUsuario: ");
    scanf("%29s", usuario);

    printf("\nSenha: ");
    scanf("%19s", senha);

    if(strcmp(usuario, "marcos.recepcao") == 0 && strcmp(senha, "123456") == 0) {
        printf("\nLogin realizado com sucesso!\n");

    //Tela da recepção aqui

    } else {
        printf("\nUsuario ou senha incorretos.\n");
    }
    
    return 0;

}