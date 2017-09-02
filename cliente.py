#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import requests



def pedirNumeroEntero():
    correcto = False
    num = 0
    while (not correcto):
        try:
            num = int(input("Introduce un numero entero: "))
            correcto = True
        except ValueError:
            print('Error, introduce un numero entero')
    return num



salir = False
opcion = 0

while not salir:

    print ("1. Listar Dispositivos")
    print ("2. Nombrar_dispositivo")
    print ("3. Leer_archivo")
    print ("4. Escribir_archivo")
    print ("5. Salir")

    print ("Elige una opcion")
    opcion = pedirNumeroEntero()

    if opcion == 1:
        print ("Bienvenido a la seccion Listar Dispositivos")
        l_d = requests.get("http://127.0.0.1:8888/listar_dispositivos")
        if l_d.status_code==200:
            print ("Conexión Establecida")

    elif opcion == 2:
        print("Bienvenidos a la sección Nombrar Archivo")
        """nodo=input('Ingrese el nodo donde esta su dispositivo: ')
        nuevo_nombre=input('Ingrese el nuevo nombre de su dispositivo')
        s_nombrar_dispositivos = {'solicitud': 'nombrar_dispositivo', 'nodo': nodo,
                                 'nuevo_nombre': nuevo_nombre}
                                 """
        s_nombrar_dispositivos = {'solicitud': 'nombrar_dispositivo', 'nodo': '/dev/user',
                                  'nuevo_nombre': 'Roberth_pen'}
        n_d = requests.post("http://127.0.0.1:8888/nombrar_dispositivo", json=json.dumps(s_nombrar_dispositivos))
        if n_d.status_code == 200:
            print('Conexion Satifactoria')
            print ("Aqui va todo, luego ire actualizando")
            #respuesta=j.json()
            #print(respuesta)

    elif opcion == 3:
        print("Bienvenidos a la sección Leer Archivo")
        l_a= requests.get("http://127.0.0.1:8888/leer_archivo")
        if l_a.status_code==200:
            print ("Conexión Establecida")

    elif opcion == 4:
        print("Bienvenidos a la sección Escribir Archivo")


        s_escribir_archivo = {'solicitud': 'escribir_archivo', 'nombre': 'kignstong',
                              'nombre_archivo': 'proyecto_programacion'}
        # este metodo me repondera a la solicitud de escribir archivo
        e_a = requests.post("http://127.0.0.1:8888/escribir_archivo", json=json.dumps(s_escribir_archivo))
        if e_a.status_code == 200:
            print('Conexion Satifactoria')
            print ("Aqui va todo, luego ire actualizando")

    elif opcion == 5:
        salir = True
    else:
        print ("****Introduce un numero entre 1 y 4******")

print ("Fin")

