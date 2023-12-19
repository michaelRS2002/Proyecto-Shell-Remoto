# Proyecto-Shell-Remoto-SO
### Descripcion
Proyecto Final Sistemas Operativos. Objetivo programar en C, una aplicación cliente/servidor que tiene como cliente al usuario que ejecuta los comandos y el servidor es el computador remoto  donde se ejecutan los comandos.
# Ejecucion de proyecto
### Requisitos
Para hacer funcionar el proyecto debemos contar con Docker y disponer de una terminal Linux. Trabajando en el sistema operativo Windows debemos contar con Windows Subsystem Linux (WSL). 
### Primer Paso
Abre una terminal tipo Linux. 
Hacemos un copia del repositorio escribiendo en la terminal asi:

`git clone https://github.com/michaelRS2002/Proyecto-Shell-Remoto`

Abrimos la carpeta y revisamos que se hayan guardado todos los archivos asi

`cd Proyecto-Shell-Remoto/`

`ls`
### Segundo Paso
Creamos un network donde se van a comunicar los contenedores cliente-servidor asi

`docker network create workspace`

Si estamos usando la aplicacion en el mismo computador dividimos la terminal con:

`tmux`

`Ctrl+B` + `Shift+2`

Para ir de un lado a otro de pantalla  `Ctrl+B` + Flecha

Creamos 2 contenedores uno  para el servidor y otro para cliente con el siguiente comando cada uno en una parte del terminal 

`docker container run --name servidor -v $(pwd):/servidor -it --network workspace ubuntu`

`docker container run --name cliente -v $(pwd):/cliente -it --network workspace ubuntu`

## Tercer Paso 
Acceder a la carpeta servidor y la carpeta cliente usa

`cd servidor`

`cd cliente`

Revisamos si tenemos archivos `ls`

Si es primera vez que lo ejecutamos escribimos `apt update && apt install -y build-essential` en ambas terminales

Compilamos

`gcc -o servidor servidor.c tcp.c leercadena.c ` 

`gcc -o cliente cliente.c tcp.c leercadena.c ` 

Ejecutamos siempre de primero el servidor

`./Servidor `

`./cliente servidor 12348 ` 
