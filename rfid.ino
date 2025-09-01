#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

// CONFIGURACIÓN DE RED Y SERVICIOS
const char* ssid = "Planta_alta";                    // Nombre de la red WiFi
const char* password = "65431993";          // Contraseña WiFi
const char* mqtt_server = "test.mosquitto.org"; // Servidor MQTT público
const int mqtt_port = 1883;                 // Puerto MQTT estándar
const char* mqtt_topic = "Equipo4/PVTP";     // Tópico MQTT para publicar

// CONFIGURACIÓN DE HORA (NTP)
const char* ntpServer = "pool.ntp.org";     // Servidor NTP para obtener hora
const long gmtOffset_sec = -6 * 3600;       // GMT-6 (Hora central México)
const int daylightOffset_sec = 0;           // Sin horario de verano

// CONFIGURACIÓN HARDWARE
#define SS_PIN 5                            // Pin SPI SS para RFID
#define RST_PIN 0                          // Pin reset para RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);           // Objeto lector RFID

#define LED_RED 25                          // Pin LED rojo (RGB cátodo común)
#define LED_GREEN 26                        // Pin LED verde
#define LED_BLUE 27                         // Pin LED azul

// ESTADOS DE TARJETA
#define ESTADO_SIN_PASE 0                   // Estado inicial sin registro
#define ESTADO_ENTRADA 1                    // Registró entrada
#define ESTADO_SALIDA 2                     // Registró salida

// ESTRUCTURA PARA INTEGRANTES
struct Integrante {
  String uid;               // ID único de la tarjeta RFID
  String nombre;            // Nombre del integrante
  int red;                  // Componente rojo del color LED
  int green;                // Componente verde del color LED
  int blue;                 // Componente azul del color LED
  int estado;               // Estado actual (entrada/salida)
  bool cicloCompleto;       // Si completó ciclo entrada+salida
};

// LISTA DE INTEGRANTES AUTORIZADOS
Integrante integrantes[] = {
  {"D3059A32", "Pedro Palomino", 0, 0, 255, ESTADO_SIN_PASE, false},   // Azul
  {"E205361D", "Luis Torres", 255, 255, 0, ESTADO_SIN_PASE, false}   // Amarillo
};

const int numIntegrantes = 2;               // Número de integrantes
String nombreEquipo = "Equipo4/PVTP";        // Nombre del equipo para MQTT

// OBJETOS PARA COMUNICACIÓN
WiFiClient espClient;                       // Cliente WiFi para ESP32
PubSubClient client(espClient);             // Cliente MQTT
unsigned long lastReconnectAttempt = 0;     // Control de reconexión MQTT
bool timeConfigured = false;                // Bandera de hora configurada

// SETUP - CONFIGURACIÓN INICIAL
void setup() {
  Serial.begin(115200);                     // Inicializar comunicación serial
  
  // Configurar pines LED como salidas y apagarlos
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  apagarLED();
  
  // Inicializar comunicación SPI y lector RFID
  SPI.begin();
  mfrc522.PCD_Init();
  
  conectarWiFi();       // Conectar a red WiFi
  configurarHora();     // Obtener hora desde servidor NTP
  
  // Configurar cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setBufferSize(512);  // Buffer para mensajes JSON
}

// LOOP - BUCLE PRINCIPAL
void loop() {
  // Mantener conexión MQTT activa
  if (!client.connected()) {
    reconnectMQTT();
  } else {
    client.loop();  // Procesar mensajes MQTT entrantes
  }

  // Detectar y procesar tarjetas RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uid = obtenerUID();  // Leer UID de la tarjeta
    Serial.println("UID detectado: " + uid);
    procesarTarjeta(uid);       // Procesar según estado
    mfrc522.PICC_HaltA();       // Poner tarjeta en reposo
    delay(1000);                // Pequeña pausa
  }
}

// CONFIGURAR HORA DESDE SERVIDOR NTP
void configurarHora() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  Serial.print("Obteniendo hora desde NTP...");
  
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    timeConfigured = true;
    Serial.println("Hora configurada");
    // Mostrar fecha y hora actual
    Serial.print("Fecha: ");
    Serial.print(&timeinfo, "%d/%m/%Y");
    Serial.print(" Hora: ");
    Serial.println(&timeinfo, "%H:%M:%S");
  } else {
    Serial.println("Error obteniendo hora");
  }
}

// OBTENER FECHA ACTUAL EN FORMATO DD/MM/AAAA
String obtenerFechaHora() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Error hora";
  }
  
  char buffer[20];
  strftime(buffer, sizeof(buffer), "%d/%m/%Y", &timeinfo);
  return String(buffer);
}

// OBTENER HORA ACTUAL EN FORMATO HH:MM:SS
String obtenerHoraActual() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Error hora";
  }
  
  char buffer[20];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
  return String(buffer);
}

// RECONECTAR AL SERVIDOR MQTT SI SE PIERDE CONEXIÓN
boolean reconnectMQTT() {
  unsigned long now = millis();
  
  // Esperar 5 segundos entre intentos
  if (now - lastReconnectAttempt < 5000) {
    return false;
  }
  lastReconnectAttempt = now;
  
  Serial.print("Intentando conexión MQTT...");
  
  // Generar ID de cliente único
  String clientId = "ESP32Client-" + String(random(0xffff), HEX);
  
  if (client.connect(clientId.c_str())) {
    Serial.println("conectado");
    Serial.print("Topic: ");
    Serial.println(mqtt_topic);
    return true;
  } else {
    Serial.print("falló, rc=");
    Serial.print(client.state());
    Serial.println(" intentando en 5 segundos");
    return false;
  }
}

// OBTENER UID DE TARJETA RFID EN FORMATO HEXADECIMAL
String obtenerUID() {
  String uid = "";
  // Convertir bytes del UID a hexadecimal
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();    // Convertir a mayúsculas
  uid.replace(" ", ""); // Eliminar espacios
  return uid;
}

// PROCESAR TARJETA RFID SEGÚN SU ESTADO ACTUAL
void procesarTarjeta(String uid) {
  int index = -1;
  String nombre = "Desconocido";
  String accion = "";
  
  // Buscar UID en lista de autorizados
  for (int i = 0; i < numIntegrantes; i++) {
    String uidAlmacenado = integrantes[i].uid;
    uidAlmacenado.replace(" ", "");
    
    if (uid == uidAlmacenado) {
      index = i;
      nombre = integrantes[i].nombre;
      break;
    }
  }
  
  if (index == -1) {
    // Tarjeta no autorizada - LED rojo
    accion = "tarjeta rechazada";
    encenderLED(255, 0, 0);
    Serial.println("La tarjeta no esta registrada");
  } else {
    Integrante &integrante = integrantes[index];
    
    if (integrante.cicloCompleto) {
      // Ciclo completo - LED blanco
      accion = "tarjeta sin pase";
      encenderLED(255, 255, 255);
      Serial.println("Tarjeta sin pase, ciclo completado");
    } else {
      // Máquina de estados: sin pase → entrada → salida → ciclo completo
      if (integrante.estado == ESTADO_SIN_PASE || integrante.estado == ESTADO_SALIDA) {
        accion = "entrada";
        integrante.estado = ESTADO_ENTRADA;
        encenderLED(integrante.red, integrante.green, integrante.blue);
        Serial.println("Entrada aprobada");
      } else if (integrante.estado == ESTADO_ENTRADA) {
        accion = "salida";
        integrante.estado = ESTADO_SALIDA;
        integrante.cicloCompleto = true;
        encenderLED(integrante.red, integrante.green, integrante.blue);
        Serial.println("Salida registrada, Ciclo completado");
      }
    }
  }
  
  // Enviar mensaje MQTT y controlar LED
  enviarMensajeJSON(nombreEquipo, nombre, uid, accion);
  delay(2000);
  apagarLED();
}

// CONTROL LED RGB (CÁTODO COMÚN - LÓGICA INVERTIDA)
void encenderLED(int r, int g, int b) {
  analogWrite(LED_RED, r);     // 0 = máximo brillo, 255 = apagado
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE, b);
}

void apagarLED() {
  analogWrite(LED_RED, 255);         // Todos en máximo = apagado
  analogWrite(LED_GREEN, 255);
  analogWrite(LED_BLUE, 255);
}

// ENVIAR MENSAJE JSON POR MQTT CON DATOS DEL EVENTO
void enviarMensajeJSON(String equipo, String integrante, String id, String accion) {
  StaticJsonDocument<256> doc;  // Documento JSON con tamaño fijo
  
  // Construir estructura JSON
  doc["nombreEquipo"] = equipo;
  doc["nombreIntegrante"] = integrante;
  doc["id"] = id;
  
  JsonObject evento = doc.createNestedObject("evento");
  evento["accion"] = accion;
  evento["fecha"] = obtenerFechaHora();  // Fecha desde NTP
  evento["hora"] = obtenerHoraActual();  // Hora desde NTP
  
  String jsonString;
  serializeJson(doc, jsonString);  // Convertir a string JSON
  
  Serial.println("JSON enviado:");
  Serial.println(jsonString);
  
  // Publicar en broker MQTT
  if (client.connected()) {
    boolean published = client.publish(mqtt_topic, jsonString.c_str());
    if (published) {
      Serial.println("Mensaje publicado en MQTT");
      Serial.print("Topic: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.println("Error al publicar en MQTT");
    }
  } else {
    Serial.println("❌ No conectado a MQTT");
  }
}

// CONECTAR A RED WiFi CON RECONEXIÓN AUTOMÁTICA
void conectarWiFi() {
  Serial.print("Conectando a WiFi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  // Intentar conexión por máximo 10 segundos
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n WiFi conectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n Error conectando WiFi");
  }
}

// RESETEAR ESTADOS DE TODOS LOS INTEGRANTES (NO USADO EN LOOP)
void resetearEstados() {
  for (int i = 0; i < numIntegrantes; i++) {
    integrantes[i].estado = ESTADO_SIN_PASE;
    integrantes[i].cicloCompleto = false;
  }
  Serial.println("Estados reseteados");
}