# Proyecto-Shell-Remoto-SO
### Descripcion
Proyecto Final Sistemas Operativos. Objetivo programar en C, una aplicación cliente/servidor que tiene como cliente al usuario que ejecuta los comandos y el servidor es el computador remoto  donde se ejecutan los comandos.
# Ejecucion de proyecto
### Requisitos
Para hacer funcionar el proyecto debemos contar con Docker y disponer de una terminal Linux. Trabajando en el sistema operativo Windows debemos contar con Windows Subsystem Linux (WSL). 
### Primer Paso
Abre una terminal tipo Linux. 
Hacemos un copia del repositorio escribiendo en la terminal asi:

`git clone https://github.com/michaelRS2002/Proyecto-Shell-Remoto-SO`

Abrimos la carpeta y revisamos que se hayan guardado todos los archivos asi

`cd Proyecto-Shell-Remoto-SO/`

`ls`
### Segundo Paso
Creamos un network donde se van a comunicar los contenedores cliente-servidor asi

`docker network create workspace`

Creamos 2 contenedores uno para el servidor y otro para cliente con el siguiente comando 

`docker container run --name servidor -v $(pwd):/servidor -it --network workspace ubuntu`

Comando `Ctrl+D`

`docker container run --name cliente -v $(pwd):/cliente -it --network workspace ubuntu`

Comando `Ctrl+D`

## Tercer Paso 
Si estamos usando la aplicacion en el mismo computador dividimos la terminal con:

`tmux`

`Ctrl+B` + `Ctrl+2`

Inicializamos el servidor: 

`docker start servidor`
