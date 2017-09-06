ClientePythonServidorDaemon
======================

Escuela Superior Politecnica del Litoral

1 - Descripción
---------------
Programa de arquitectura cliente-servidor que monitorea los puertos usb de nuestra computadora, detecta conexiones de memorias usb a los mismos y permite renombrarlas, listarlas y leer/escribir archivos en ellas.

2 - Instalación
----------------
Para en funcionamiento de este programa es necesario contar con las siguientes librerias:

* Libreria libudev
* Libreria libmicrohttpd
* Libreria Requests

```
sudo apt-get install libudev-dev
sudo apt-get install libmicrohttpd*
sudo pip install request
pip install requests
```

Para crear los ejecutables se disponible de una archivo makefile que le facilitará el trabajo.
para la crear el ejecutable del daemonUSB y el servidor web.

```
make
```

3 - Modo de uso general
------------------------

* iniciar USB-daemon.

El daemon por defecto escuchará las solicitudes servidor web por el puerto número 8888
Este proceso se encargará de responder las solicitudes del servidor web.

```
./bin/USB-daemon
```	

* iniciar servidor web.

El servidor web actuará de intermediara entre el cliente y el proceso daemon.
Recibe la solicitud del cliente, la procesa y envia la solicutud al proceso daemon, luego recibe respuesta del daemon de la solicitud realiza, la procesa y la envia al cliente. Al ejecutarlo se debe enviar de parametro el puerto . debe ser diferente al puerto 8888 con el cual esta escuchando el procesodaemon.

```
./bin/ServidorWeb 8889
```	
No cerrar esta terminal.

Una vez realizado esto. Se pueden realizar solicitudes desde el cliente python.

Formato general solicitud.
El puerto debe ser mismo que se agrego de parametro al ejecutar el servidor web.

```
python ./bin/cliente.py <puerto| 8889> <metodo> <tipo solicitud> <arg 1> <arg 2>.....<arg n>
```	


* Listar dispositivos
Permite obtener una lista detallada de todos los dispositivos USB conectados
```
python ./src/cliente.py <puerto|8889> GET listar_dispositivos
```	
ejemplo:
```
python ./src/cliente.py 8889 GET listar_dispositivos
```	


* En caso que desee finalizar el proceso daemon.

```
ps -A | grep USB-daemon
```	
Al ejecutar esa linea se le mostrará una lista de los dispositivos que se ajusten a ese nombre.
identifique el numero (pid) del proceso llamado USB-daemon.

```
kill -TERM <NUMERO PID>
```	


4 - Autores
-----------

* Ramirez Sebastian
* Vasconez Kevin
* Loor Robert
