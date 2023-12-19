#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tcp.h"
#include "leercadena.h"

// Definiciones de colores para la terminal
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define MAX_COMMAND_LENGTH 100
#define MAX_RESPONSE_LENGTH BUFSIZ

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "%sUso: %s <ip_servidor> <puerto>%s\n", ANSI_COLOR_RED, argv[0], ANSI_COLOR_RESET);
        return 1;
    }

    char *serverIP = argv[1];
    int serverPort = atoi(argv[2]);

    int clientSocket = TCP_Open(serverIP, serverPort);

    if (clientSocket == -1) {
        fprintf(stderr, "%sError al intentar conectar. Reintentando en 1 segundo...%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
        return 1;
    }

    while (1) {
        char comando[MAX_COMMAND_LENGTH];
        char response[MAX_RESPONSE_LENGTH];
        bzero(comando, MAX_COMMAND_LENGTH);

        printf("%sDigite su comando (escriba '%ssalida%s' para salir): %s", ANSI_COLOR_BLUE, ANSI_COLOR_RED, ANSI_COLOR_BLUE, ANSI_COLOR_RESET);
        leer_de_teclado(MAX_COMMAND_LENGTH, comando);

        TCP_Write_String(clientSocket, comando);

        if (strcmp(comando, "salida") == 0) {
            printf("Cerrando conexión con el servidor...\n");
            break;
        }

        if (strncmp(comando, "file", 4) == 0) { // Si el comando comienza con "file "
            char *subcommand = strchr(comando, ' ');
            if (subcommand != NULL) {
                subcommand++; // Avanzar al siguiente carácter después del espacio

                // Manejo de comandos relacionados con archivos
                if (strncmp(subcommand, "create", 6) == 0 || strncmp(subcommand, "edit", 4) == 0 || strncmp(subcommand, "delete", 6) == 0) {
                    char *filename = strchr(subcommand, ' ');
                    if (filename != NULL) {
                        filename++; // Avanzar al nombre del archivo después del espacio

                        // Enviar el nombre del archivo al servidor
                        TCP_Write_String(clientSocket, comando);
                        printf("Comando de archivo enviado al servidor: %s\n", comando);

                        // Leer la respuesta del servidor
                        TCP_Read_String(clientSocket, response, MAX_RESPONSE_LENGTH);

                        // Manejar la respuesta del servidor para diferentes operaciones de archivos
                        if (strcmp(response, "Archivo creado con éxito.") == 0) {
                            printf("El archivo fue creado con éxito. Para editarlo, usa 'file edit <filename>'.\n");
                            continue;
                        } else if (strcmp(response, "Puedes editar el archivo con nano") == 0) {
                            // Abrir nano para editar el archivo recibido directamente
                            char comandoEdit[100];
                            sprintf(comandoEdit, "nano %s", filename);
                            system(comandoEdit);
                            continue;
                        } else if (strcmp(response, "El archivo no existe en el servidor.") == 0) {
                            printf("El archivo no existe en el servidor.\n");
                            continue;
                        } else {
                            printf("Error en la operación de archivo.\n");
                            continue;
                        }
                    } else {
                        printf("Falta el nombre del archivo.\n");
                        continue;
                    }
                }
            }
        }

        // Resto del código para manejar otros tipos de comandos
        // ...

        bzero(comando, MAX_COMMAND_LENGTH);
    }

    TCP_Close(clientSocket);
    return 0;
}
