#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tcp.h"
#include "leercadena.h"

#define MAX_COMMAND_LENGTH 100
#define MAX_RESPONSE_LENGTH BUFSIZ

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "%sUso: %s <ip_servidor> <puerto>%s\n", ANSI_COLOR_RED, argv[0], ANSI_COLOR_RESET);
    return 1;
  }

  char *serverIP = argv[1];
  int serverPort = atoi(argv[2]);

  int clientSocket = TCP_Open(serverIP, serverPort);

  if (clientSocket == -1)
  {
    fprintf(stderr, "%sError al intentar conectar. Reintentando en 1 segundo...%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    return 1;
  }

  while (1)
  {
    char comando[MAX_COMMAND_LENGTH];
    bzero(comando, MAX_COMMAND_LENGTH);

    printf("%sDigite su comando (escriba '%ssalida%s' para salir):> %s", ANSI_COLOR_BLUE, ANSI_COLOR_RED, ANSI_COLOR_BLUE, ANSI_COLOR_RESET);
    leer_de_teclado(MAX_COMMAND_LENGTH, comando);

    TCP_Write_String(clientSocket, comando);

    if (strcmp(comando, "salida") == 0)
    {
      printf("Cerrando conexion con servidor...\n");
      break;
    }
       if (strncmp(comando, "create", 6) == 0) { // Si el comando comienza con "create"
    char *filename = strchr(comando, ' ');
      if (filename != NULL) {
        filename++; // Avanzar al nombre del archivo después del espacio

        // Enviar el nombre del archivo al servidor
        TCP_Write_String(clientSocket, filename);
        printf("Crear archivo: %s\n", filename);

        char response[MAX_RESPONSE_LENGTH];
        TCP_Read_String(clientSocket, response, MAX_RESPONSE_LENGTH);

        // Si el servidor envía la señal para editar con nano
        if (strcmp(response, "Se ha creado el archivo") == 0) {
            // Archivo creado correctamente
            printf("El archivo se creo para editarlo escribir edit %s\n", filename);
            continue;
        }
        if (strcmp(response, "El archivo ya existe.") == 0) {
            // Archivo ya existe
            printf("El archivo ya existe puede editarlo con edit %s\n", filename);
            continue;
        }
        if (strcmp(response, "El archivo no se pudo crear en el servidor.") == 0) {
            // Archivo no se pudo crear
            printf("Error no se pudo crear archivo.\n");
            continue;
        }
      }
    }
    if (strncmp(comando, "edit", 4) == 0) { // Si el comando comienza con "edit "
    char *filename = strchr(comando, ' ');
      if (filename != NULL) {
        filename++; // Avanzar al nombre del archivo después del espacio

        // Enviar el nombre del archivo al servidor
        TCP_Write_String(clientSocket, filename);
        printf("Enviado nombre de archivo: %s\n", filename);

        char response[MAX_RESPONSE_LENGTH];
        TCP_Read_String(clientSocket, response, MAX_RESPONSE_LENGTH);

        // Si el servidor envía la señal para editar con nano
        if (strcmp(response, "Puedes editar el archivo con nano") == 0) {
            // Abrir nano para editar el archivo recibido directamente
            char comandoEdit[100];
            sprintf(comandoEdit, "nano %s", filename);
            system(comandoEdit);
            continue;
        }
      }
    }
    if (strncmp(comando, "delete", 6) == 0) { // Si el comando comienza con "delete "
    char *filename = strchr(comando, ' ');
      if (filename != NULL) {
        filename++; // Avanzar al nombre del archivo después del espacio

        // Enviar el nombre del archivo al servidor
        TCP_Write_String(clientSocket, filename);
        printf("Borrar archivo: %s\n", filename);

        char response[MAX_RESPONSE_LENGTH];
        TCP_Read_String(clientSocket, response, MAX_RESPONSE_LENGTH);

        // Si el servidor envía la señal para editar con nano
        if (strcmp(response, "El Archivo ha sido borrado.") == 0) {
            // Abrir nano para editar el archivo recibido directamente
            printf("El archivo %s\n ha sido borrado del servidor.\n", filename);
            continue;
        }
        if(strcmp(response,"No se pudo borrar el archivo.") == 0){
          printf("El archivo %s\n no ha podido ser borrado.\n", filename);
          continue;
        }
      }
    }
    

    char response[MAX_RESPONSE_LENGTH];
    bzero(response, MAX_RESPONSE_LENGTH);

    // Leer hasta encontrar la marca de fin de respuesta
    while (1)
    {
      TCP_Read_String(clientSocket, response, MAX_RESPONSE_LENGTH);
      if (strcmp(response, "$") == 0)
      {
        break;
      }
      printf("%s -> \n%s%s\n", ANSI_COLOR_GREEN, response, ANSI_COLOR_RESET);
      bzero(response, MAX_RESPONSE_LENGTH);
    }
    bzero(comando, MAX_COMMAND_LENGTH);
  }

  TCP_Close(clientSocket);
  return 0;
}
