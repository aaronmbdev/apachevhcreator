
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int usage() {
    printf("Apache VH Creator\n");
    printf("Uso: ./VHCreator midominio.com nombrefolder \n");
    printf("Esto creará la configuración para midominio.com en /var/www/nombrefolder \n");
    printf("Si no se especifica nombrefolder, se usa el nombre del dominio \n");
    exit(0);
}

int main(int argc, char *argv[]) {
    char* dominio = "null";
    char* folder = "null";
    
    if(argc == 1 || argc > 3) {
        usage();
    } else {
        dominio = argv[1];
        folder = argv[1];
        if(argc == 3) {
            folder = argv[2];
        }
  
    }
}