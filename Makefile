INCLUDE=-Iinclude/
LIBS=-Llib/

all: server procesodaemon
	
server: src/server.c
	gcc -Wall src/server.c -o bin/server -lmicrohttpd

procesodaemon: funciones.o daemon_USB.o
	gcc -Wall -Iinclude/ obj/*[!1].o -ludev -o bin/exec

funciones.o: src/funciones.c
	gcc -Wall -c  -Iinclude/ -ludev src/funciones.c -o obj/funciones.o

daemon_USB.o: src/daemon_USB.c
	gcc -Wall -c  -ludev  -pthread -Iinclude/  src/daemon_USB.c -o obj/daemon_USB.o



.PHONY: clean
clean:
	rm -rf obj/* bin/* lib/* log/usb.log
