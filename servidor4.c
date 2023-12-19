#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tcp.h"
#include "leercadena.h"

#define SERVER_PORT 12348
#define MAX_RESPONSE_LENGTH BUFSIZ

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_GREEN "\x1b[32m"

void handleFileOperation(int clientSocket, const char *subcommand) {
  char *filename = strchr(subcommand, ' ');

  if (filename != NULL) {
    filename++; // Avanzar al siguiente carácter después del espacio

    if (strncmp(subcommand, "create", 6) == 0) {
      FILE *file = fopen(filename, "a");
      if (file != NULL) {
        fclose(file);
        TCP_Write_String(clientSocket, "Archivo creado con éxito.");
        printf("Archivo '%s' creado con éxito.\n", filename);
      } else {
        printf("Error al crear el archivo '%s'.\n", filename);
        TCP_Write_String(clientSocket, "Error al crear el archivo.");
      }
    } else if (strncmp(subcommand, "edit", 4) == 0) {
      printf("Archivo a editar: %s\n", filename);
      if (access(filename, F_OK) != -1) {
        TCP_Write_String(clientSocket, "Puedes editar el archivo con nano");
      } else {
        printf("El archivo '%s' no existe o no se puede acceder.\n", filename);
        TCP_Write_String(clientSocket, "El archivo no existe en el servidor.");
      }
    } else if (strncmp(subcommand, "delete", 6) == 0) {
      printf("Archivo a borrar: %s\n", filename);
      if (remove(filename) == 0) {
        printf("Archivo '%s' eliminado exitosamente.\n", filename);
        TCP_Write_String(clientSocket, "Archivo eliminado exitosamente.");
      } else {
        printf("Error al intentar eliminar el archivo '%s'.\n", filename);
        TCP_Write_String(clientSocket, "Error al intentar eliminar el archivo.");
      }
    } else {
      TCP_Write_String(clientSocket, "Comando no reconocido para el archivo.");
    }
  } else {
    TCP_Write_String(clientSocket, "Falta el nombre del archivo.");
  }
}

void handleGeneralCommand(int clientSocket, const char *command) {
  pid_t pid = fork();

  if (pid == 0) {
    dup2(clientSocket, STDOUT_FILENO);
    dup2(clientSocket, STDERR_FILENO);
    close(clientSocket);
    execlp("/bin/sh", "/bin/sh", "-c", command, NULL);
    exit(0);
  } else if (pid > 0) {
    waitpid(pid, NULL, 0);
    TCP_Write_String(clientSocket, "$");
  } else {
    TCP_Write_String(clientSocket, ANSI_COLOR_RED "Error al ejecutar el comando." ANSI_COLOR_RESET);
  }
}

int main() {
  int serverSocket = TCP_Server_Open(SERVER_PORT);
  int clientSocket = TCP_Accept(serverSocket);

  while (1) {
    char command[BUFSIZ];
    TCP_Read_String(clientSocket, command, BUFSIZ);

    if (strcmp("salida", command) == 0) {
      TCP_Write_String(clientSocket, ANSI_COLOR_GREEN "Cerrando conexion..." ANSI_COLOR_RESET);
      break;
    }

    if (strncmp(command, "file", 4) == 0) {
      char *subcommand = strchr(command, ' ');
      if (subcommand != NULL) {
        subcommand++; // Avanzar al siguiente carácter después del espacio
        handleFileOperation(clientSocket, subcommand);
      } else {
        TCP_Write_String(clientSocket, "Falta el nombre del archivo.");
      }
    } else {
      handleGeneralCommand(clientSocket, command);
    }
    
    bzero(command, BUFSIZ);
  }

  TCP_Close(clientSocket);
  TCP_Close(serverSocket);
  return 0;
}
