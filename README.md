# Proyecto-IoT-LORE-MK-1
Para el correcto funcionamiento de estos códigos, es necesario descargar e instalar previamente las siguientes librerías:

IRremote.h
Arduino.h
ESP8266WiFi.h
SinricPro.h
SinricProSwitch.h

Este proyecto comprende de dos partes de código:

Parte 1: Programación de NodeMCU
------------------------------------------------------------------------------------------------------------------------------------------------------------------
Este código está completamente basado en el códidgo arduino presnetado en el video de Tech StudyCell (https://www.youtube.com/watch?v=gpB4600keWA) (Publicado el 7 de Abril del 2021). Cabe recalcar que las credenciales de WiFi y de dispositivos "device ID" cambiará para cada individuo.

Los "Device ID" , así como la "APP_KEY" y "APP_SECRET" deberán obtenerse directamente registrándose en la página web de "Sinric Pro" (https://sinric.pro/es-index.html). 

El código aún mantiene capacidades para incluir switches para control manual. En el contexto del proyeco IoT LORE MK-1, estos no se toman en cuenta.


Parte 2: Programación Para Arduino IDE
-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Esta segunda parte está basada en el ejemplo "SimpleSender" de la librería "IRremote", la cual tiene modificaciones originales como lo son: La implementación de los ciclos "while" dentro de los condicionales, y los contadores "aux" y "aux2". Estos permiten controlar el orden de los comandos recibidos. Es decir, que son los que permiten programar los ciclos de encendido y aagado de un electrodoméstico que funcione con un control de infrarrojos. 
