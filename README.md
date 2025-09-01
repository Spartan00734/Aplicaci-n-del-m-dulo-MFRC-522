[README_RFID_ESP32.md](https://github.com/user-attachments/files/22084688/README_RFID_ESP32.md)
README - Proyecto ESP32 con Lector RFID MFRC522 y MQTT
Equipo 4 Integrantes:
Pedro Iván Palomino Viera
Luis Antonio Torres Padrón
# 1. Objetivo General
Implementar un sistema de control de accesos utilizando un lector RFID MFRC522 conectado a un ESP32, que registre eventos de entrada y salida en tiempo real y los envíe mediante el protocolo MQTT en formato JSON.
# 2. Objetivos Específicos
Configurar el módulo MFRC522 con el ESP32 para la lectura de tarjetas RFID.
Establecer la conexión WiFi y sincronizar la hora mediante NTP.
Integrar la comunicación con un broker MQTT para el envío de datos.
Diseñar la estructura del mensaje en formato JSON con los campos requeridos.
Validar el funcionamiento del sistema mediante pruebas de lectura y publicación de mensajes.
# 3. Competencias
• Manejo de hardware de comunicación RFID.
• Configuración de redes inalámbricas en microcontroladores.
• Uso del protocolo MQTT para IoT.
• Serialización y envío de datos en formato JSON.
• Implementación de buenas prácticas de programación en sistemas embebidos.

# 4. Tabla de Contenidos
# 5. Descripción
Esta práctica permite a los estudiantes comprender cómo integrar un lector RFID con un ESP32 para gestionar accesos en entornos inteligentes. El sistema valida tarjetas autorizadas, enciende un LED RGB según el estado y registra eventos de entrada/salida. Los datos se envían en tiempo real a un broker MQTT, lo que facilita la integración con aplicaciones IoT y paneles de monitoreo.
# 6. Requisitos
• Hardware necesario:
- ESP32 DevKit v1
- Módulo RFID MFRC522
- Tarjetas/Tags RFID
- LED RGB (cátodo común)
- Cables Dupont
- Protoboard

• Software y bibliotecas requeridas:
- Arduino IDE
- Biblioteca MFRC522
- WiFi.h
- PubSubClient.h
- ArduinoJson.h

• Conocimientos previos imprescindibles:
- Fundamentos de programación en C/C++ para Arduino
- Conceptos básicos de redes WiFi
- Uso de protocolos de comunicación IoT (MQTT)
- Manejo de estructuras JSON
# 7. Instalación y Configuración
Clonar o descargar el repositorio con el código fuente.
Abrir el proyecto en Arduino IDE.
Instalar las bibliotecas requeridas desde el Gestor de Librerías.
Configurar las credenciales WiFi en el código (SSID y Password).
Verificar la configuración de pines de conexión del módulo RFID y LEDs.
Cargar el programa en la tarjeta ESP32.




# 8. Conexiones de Hardware
# 9. Parámetros Técnicos del MFRC-522
# 10. Uso y ejemplos de Código
El código fuente debe insertarse en el archivo principal de Arduino IDE. Cada bloque debe estar comentado explicando su función utilizando “//”, por ejemplo: // configuración WiFi, // conexión al broker MQTT, // lectura de tarjetas RFID, // procesamiento de estados y envío de mensajes JSON, etc.
# 11. Resultados de Prueba
Los resultados deben documentarse incluyendo las lecturas de tarjetas, mensajes publicados en el broker MQTT y el encendido del LED RGB. La salida puede observarse en la consola serial, en el broker MQTT y en aplicaciones clientes de IoT.
# 12. Consideraciones Éticas y de Seguridad
• Proteger la privacidad de los usuarios al no exponer identificadores personales.
• Prevenir accesos no autorizados mediante cifrado en la comunicación MQTT.
• Evitar almacenamiento inseguro de credenciales WiFi en el código.
• Evaluar vulnerabilidades en la clonación de tarjetas RFID.
• Garantizar un uso responsable del sistema en entornos educativos.
# 13. Formato de Salida (JSON)

# 14. Solución de Problemas
# 15. Contribuciones
El flujo de trabajo para aportar mejoras al proyecto es el siguiente:
•

# 16. Referencias
• MFRC522 Datasheet. NXP Semiconductors.
• PubSubClient Library Documentation - https://pubsubclient.knolleary.net/
• ArduinoJson Library Documentation - https://arduinojson.org/
• ESP32 Arduino Core Documentation - https://docs.espressif.com/
• Protocolo MQTT - OASIS Standard.

| Señal del módulo | Pin de la placa (ESP32) | Función |
| --- | --- | --- |
| SDA/SS | GPIO 5 | Chip Select RFID |
| RST | GPIO 0 | Reset RFID |
| MOSI | GPIO 23 | SPI MOSI |
| MISO | GPIO 19 | SPI MISO |
| SCK | GPIO 18 | SPI Clock |
| 3.3V | 3.3V | Alimentación |
| GND | GND | Tierra |
| LED Rojo | GPIO 25 | Indicador acceso denegado |
| LED Verde | GPIO 26 | Indicador acceso permitido |
| LED Azul | GPIO 27 | Indicador de estado |


| Parámetro | Valor típico | Unidad |
| --- | --- | --- |
| Voltaje de operación | 3.3 | V |
| Corriente típica | 13-26 | mA |
| Interfaz de comunicación | SPI / I2C / UART | - |
| Frecuencia de operación | 13.56 | MHz |


| Campo | Tipo de dato | Descripción |
| --- | --- | --- |
| nombreEquipo | String | Identificador del equipo que envía el dato |
| nombreIntegrante | String | Nombre del usuario registrado |
| id | String | UID de la tarjeta RFID |
| evento.accion | String | Acción realizada (entrada/salida/rechazo) |
| evento.fecha | String | Fecha en formato DD/MM/AAAA |
| evento.hora | String | Hora en formato HH:MM:SS |


| Problema común | Solución recomendada |
| --- | --- |
| No conecta a WiFi | Verificar SSID y contraseña, revisar intensidad de señal. |
| Falla al conectar al broker MQTT | Comprobar dirección y puerto del servidor, validar conexión a internet. |
| No se leen las tarjetas RFID | Revisar conexiones SPI y alimentación del módulo MFRC522. |
| LED RGB no enciende correctamente | Verificar polaridad y pines configurados en el código. |
| JSON no se publica | Asegurar que el cliente MQTT está conectado antes de publicar. |
