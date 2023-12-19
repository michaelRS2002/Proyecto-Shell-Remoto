#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "doc_code/tcp.h"
#include "doc_code/leercadena.h"

#define SERVER_PORT 12348
#define MAX_RESPONSE_LENGTH BUFSIZ

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_GREEN "\x1b[32m"

int main()
{
  int serverSocket = TCP_Server_Open(SERVER_PORT);
  int clientSocket = TCP_Accept(serverSocket);

  while (1)
  {
    char command[BUFSIZ];
    TCP_Read_String(clientSocket, command, BUFSIZ);

    if (strcmp("salida", command) == 0)
    {
      TCP_Write_String(clientSocket, ANSI_COLOR_GREEN "Cerrando conexion..." ANSI_COLOR_RESET);
      break;
    }
    pid_t pid = fork();

    if (pid == 0)
    {
      // Redirigir la salida estándar al socket
      dup2(clientSocket, STDOUT_FILENO);
      dup2(clientSocket, STDERR_FILENO);

      // Cerrar el socket de lectura en el proceso hijo
      close(clientSocket);

      // Ejecutar el comando y salir
      execlp("/bin/sh", "/bin/sh", "-c", command, NULL);
      exit(0);
    }
    else if (pid > 0)
    {
      // Padre: esperar que el proceso hijo termine antes de continuar
      waitpid(pid, NULL, 0);
      // Enviar marca de fin de respuesta
      TCP_Write_String(clientSocket, "$");
    }
    else
    {
      // Ocurrió un error al intentar crear el proceso hijo
      TCP_Write_String(clientSocket, ANSI_COLOR_RED "Error al ejecutar el comando." ANSI_COLOR_RESET);
    }
    bzero(command, BUFSIZ);
  }

  TCP_Close(clientSocket);
  TCP_Close(serverSocket);
  return 0;
}
