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

void manejo_comando_edit(int clientSocket, const char *filename) {
    // Envía el mensaje al cliente
    TCP_Write_String(clientSocket, "Puedes editar el archivo con nano");
    // Ejecuta nano en el cliente y espera a que termine
     char response[MAX_RESPONSE_LENGTH];
     TCP_Read_String(clientSocket, response, MAX_RESPONSE_LENGTH);
     if (strcmp(response, "Edicion terminada") == 0){
    // Envía la marca de fin de respuesta al cliente
	Printf("Edicion por parte del cliente completada");     
    	TCP_Write_String(clientSocket, "Recibido");
     }
}


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
    if (strncmp(command, "create", 6) == 0) {
      char *filename = strchr(command,' ');
    if (filename != NULL) {
        filename++; // Avanzar al siguiente carácter después del espacio
        printf("Archivo a crear: %s\n", filename);
        
        if (access(filename, F_OK) != -1) {
            // El archivo existe
            TCP_Write_String(clientSocket, "El archivo ya existe.");
        } else {
            // Intentamos crear el archivo
            FILE* archivo = fopen(filename, "w");
            if (archivo != NULL) {
                printf("Se ha creado el archivo '%s'.\n", filename);
                TCP_Write_String(clientSocket, "Se ha creado el archivo");
                fclose(archivo);
            } else {
                printf("No se pudo crear el archivo '%s'.\n", filename);
                TCP_Write_String(clientSocket, "El archivo no se pudo crear en el servidor.");
            }
        }
    }
}
    if (strncmp(command, "edit", 4) == 0) {
            char *filename = strchr(command, ' ');
            if (filename != NULL) {
                filename++; // Avanzar al siguiente carácter después del espacio
                printf("Archivo a editar: %s\n", filename);
		manejo_comando_edit(clientSocket,filename);
            }
    }    
     if (strncmp(command, "delete", 4) == 0) {
            char *filename = strchr(command, ' ');
            if (filename != NULL) {
                filename++; // Avanzar al siguiente carácter después del espacio
                printf("Archivo a borrar: %s\n", filename);
                if (access(filename, F_OK) != -1) {
                    // El archivo existe, se borra
                    if (remove(filename) == 0) {
                      printf("El archivo '%s' ha sido borrado.\n", filename);
                      TCP_Write_String(clientSocket, "El Archivo ha sido borrado.");
                    } else {
                     printf("No se pudo borrar el archivo '%s'.\n", filename);
                     TCP_Write_String(clientSocket, "No se pudo borrar el archivo."); 
                    }
               }
            }
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
