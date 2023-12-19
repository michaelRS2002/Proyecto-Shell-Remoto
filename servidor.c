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
    if (strncmp(command, "file", 4) == 0) { // Si el comando comienza con "file "
        char *subcommand = strchr(command, ' '); // Buscar el primer espacio en el comando
        if (subcommand != NULL) {
          subcommand++; // Avanzar al siguiente carácter después del espacio
          if (strncmp(subcommand, "create", 6) == 0) {
              char *filename = strchr(subcommand, ' ');
              if (filename != NULL) {
                  filename++; // Avanzar al siguiente carácter después del espacio
                  FILE *file = fopen(filename, "a");
                  if (file != NULL) {
                    fclose(file);
                    TCP_Write_String(clientSocket, "Archivo creado con éxito.");
                    printf("Archivo '%s' creado con éxito.\n", filename);
                  } else {
                    printf("Error al crear el archivo");
                    TCP_Write_String(clientSocket, "Error al crear el archivo.");
                    printf("Error al crear el archivo '%s'.\n", filename);
                    }
              }
          }
        if (strncmp(subcommand, "edit", 4) == 0) {
            char *filename = strchr(subcommand, ' ');
            if (filename != NULL) {
                filename++; // Avanzar al siguiente carácter después del espacio
                printf("Archivo a editar: %s\n", filename);
                if (access(filename, F_OK) != -1) {
                  TCP_Write_String(clientSocket, "Puedes editar el archivo con nano");
                } 
                else {
                printf("El archivo que pide cliente no existe o no se puede acceder.\n");
                TCP_Write_String(clientSocket, "El archivo no existe en el servidor.");
                }
            }
        }        
        if (strncmp(subcommand, "delete", 6) == 0) {
            char *filename = strchr(subcommand, ' ');
            if (filename != NULL) {
                filename++; // Avanzar al siguiente carácter después del espacio
                printf("Archivo a borrar: %s\n", filename);
                if (remove(filename) == 0) {
                  printf("Archivo eliminado exitosamente.\n");
                  TCP_Write_String(clientSocket, "Archivo eliminado exitosamente.\n" );
                 } else {
                   printf("Error al intentar eliminar el archivo.\n");
                   }
                }
              }
           }else {
            TCP_Write_String(clientSocket, "Comando no reconocido para el archivo.");
        }else {
        TCP_Write_String(clientSocket, "Falta el nombre del archivo.");
        }
      }
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
    }else
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

/* #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../doc_code/tcp.h"
#include "../doc_code/leercadena.h"

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
      FILE *fp = popen(command, "r");
      char response[MAX_RESPONSE_LENGTH];
      while (fgets(response, 1024, fp) != NULL)
      {
        TCP_Write_String(clientSocket, response);
      }

      TCP_Write_String(clientSocket, "$");
      pclose(fp);
      exit(0);
    }
    else if (pid > 0)
    {
      // Padre: esperar que el proceso hijo termine antes de continuar
      waitpid(pid, NULL, 0);
    }
    else
    {
      // Ocurrió un error al intentar crear el proceso hijo
      TCP_Write_String(clientSocket, ANSI_COLOR_RED "Error al ejecutar el comando." ANSI_COLOR_RESET);
    }
  }

  TCP_Close(clientSocket);
  TCP_Close(serverSocket);
  return 0;
} */

/* #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../doc_code/tcp.h"
#include "../doc_code/leercadena.h"

#define SERVER_PORT 12348
#define MAX_RESPONSE_LENGTH BUFSIZ

#define ANSI_COLOR_RED "\x1b[31m"

int main()
{
  int serverSocket = TCP_Server_Open(SERVER_PORT);
  int clientSocket = TCP_Accept(serverSocket);

  while (1)
  {
    char command[BUFSIZ];
    bzero(command, BUFSIZ);
    TCP_Read_String(clientSocket, command, BUFSIZ);

    FILE *stream = popen(command, "r");
    if (stream == NULL)
    {
      // perror("Error al ejecutar el comando");
      TCP_Write_String(clientSocket, ANSI_COLOR_RED "Error al ejecutar el comando. Verifique la sintaxis y vuelva a intentarlo.");
      continue;
    }

    if (strcmp(command, "salida\n") == 0)
    {
      break;
    }

    char response[MAX_RESPONSE_LENGTH];
    bzero(response, MAX_RESPONSE_LENGTH);
    size_t bytesRead;

    while ((bytesRead = fread(response, sizeof(char), sizeof(response) - 1, stream)) > 0)
    {
      response[bytesRead] = '\0';
      TCP_Write_String(clientSocket, response);
    }

    TCP_Write_String(clientSocket, "$");

    pclose(stream);
  }

  TCP_Close(clientSocket);
  TCP_Close(serverSocket);
  return 0;
}
 */
/* #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../doc_code/tcp.h"
#include "../doc_code/leercadena.h"

#define SERVER_PORT 12348
#define MAX_RESPONSE_LENGTH BUFSIZ

#define ANSI_COLOR_RED "\x1b[31m"

int main()
{
  int serverSocket = TCP_Server_Open(SERVER_PORT);
  int clientSocket = TCP_Accept(serverSocket);

  while (1)
  {
    char command[BUFSIZ];
    TCP_Read_String(clientSocket, command, BUFSIZ);

    FILE *stream = popen(command, "r");
    if (stream == NULL)
    {
      //perror("Error al ejecutar el comando");
      TCP_Write_String(clientSocket, ANSI_COLOR_RED "Error al ejecutar el comando. Verifique la sintaxis y vuelva a intentarlo.");
      continue;
    }

    if (strcmp(command, "salida\n") == 0)
    {
      break;
    }

    char response[MAX_RESPONSE_LENGTH];
    size_t bytesRead;

    while ((bytesRead = fread(response, sizeof(char), sizeof(response) - 1, stream)) > 0)
    {
      response[bytesRead] = '\0';
      TCP_Write_String(clientSocket, response);
    }

    TCP_Write_String(clientSocket, "$");

    pclose(stream);
  }

  TCP_Close(clientSocket);
  TCP_Close(serverSocket);
  return 0;
} */

/* #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../doc_code/tcp.h"
#include "../doc_code/leercadena.h"

#define SERVER_PORT 12346
#define MAX_RESPONSE_LENGTH BUFSIZ

int main()
{
  int serverSocket = TCP_Server_Open(SERVER_PORT);
  int clientSocket = TCP_Accept(serverSocket);

  while (1)
  {
    char command[BUFSIZ];
    TCP_Read_String(clientSocket, command, BUFSIZ);

    printf("Comando recibido: %s\n", command);

    FILE *stream = popen(command, "r");
    if (stream == NULL)
    {
      perror("Error al ejecutar el comando");
      TCP_Write_String(clientSocket, "Error al ejecutar el comando. Verifique la sintaxis y vuelva a intentarlo.");
      continue;
    }

    char response[MAX_RESPONSE_LENGTH];
    size_t bytesRead = fread(response, sizeof(char), sizeof(response) - 1, stream);
    response[bytesRead] = '\0'; // Aseguramos que la cadena esté terminada correctamente

    // Si no hay salida del comando, enviamos una respuesta vacía
    if (bytesRead == 0)
    {
      TCP_Write_String(clientSocket, "$");
    }
    else
    {
      TCP_Write_String(clientSocket, response);
    }

    if (strcmp(command, "salida\n") == 0)
    {
      break;
    }
  }

  TCP_Close(clientSocket);
  TCP_Close(serverSocket);
  return 0;
}
 */
