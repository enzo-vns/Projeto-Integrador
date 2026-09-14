#include <stdio.h>
#include <stdlib.h>

int main(){
    char usuario[15];
    char senha[8];

    printf("=================\nRECANTO DO BOSQUE\n=================");
    printf("\nAutenticacao de funcionario");

    printf("\n\nUsuario: ");
    scanf("%14s", usuario);

    printf("\nSenha: ");
    scanf("%7s", senha);

    return 0;

}