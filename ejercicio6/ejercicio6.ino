/*
 *  Bienvenidos a Sys On Chip
 * ==============================
 * Ejercicio No.6
 *
 * Sistema de monitoreo ambiental IoT completo
 *
 * Sensores utilizados:
 *  - MQ135 (calidad del aire)
 *  - MQ9 (gas)
 *  - Sensor de humedad del suelo
 *  - SI7021 (temperatura y humedad)
 *  - BH1750 (sensor de luz)
 *
 * Módulos adicionales:
 *  - RTC DS3231 (fecha y hora)
 *  - Pantalla OLED SSD1306
 *  - Tarjeta microSD para registro CSV
 *  - Comunicación WiFi con Raspberry Pi
 *
 * Arquitectura:
 *
 *  Sensores → Arduino → 
 *      ├ OLED
 *      ├ microSD
 *      └ WiFi → Raspberry Pi → CSV
 *
 */


// ============================================================
// LIBRERIAS
// ============================================================

#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include <Adafruit_Si7021.h>
#include <BH1750.h>
#include "RTClib.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFiS3.h>
#include <ArduinoHttpClient.h>


// ============================================================
// CONFIGURACION OLED
// ============================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 8

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// ============================================================
// PINES ANALOGICOS
// ============================================================

const int pinSensorAire  = A0;
const int pinSensorGas   = A1;
const int pinSensorSuelo = A2;


// ============================================================
// PIN TARJETA SD
// ============================================================

const int pinSD = 10;


// ============================================================
// OBJETOS DE SENSORES
// ============================================================

Adafruit_Si7021 sensorAmbiente = Adafruit_Si7021();
BH1750 sensorLuz;
RTC_DS3231 relojRTC;

File archivoDatos;


// ============================================================
// CONFIGURACION WIFI
// ============================================================

const char nombreRed[] = "sysonchip_2.4";
const char claveRed[]  = "HjQa21082as";

const char direccionServidor[] = "192.168.0.106";
int puertoServidor = 5000;

WiFiClient clienteWiFi;
HttpClient clienteHTTP = HttpClient(clienteWiFi, direccionServidor, puertoServidor);


// ============================================================
// FUNCION DOS DIGITOS
// ============================================================

void imprimirDosDigitos(int numero)
{

  if (numero < 10)
  {
    Serial.print("0");
  }

  Serial.print(numero);
}


// ============================================================
// CREAR CABECERA CSV
// ============================================================

void crearCabeceraCSV()
{

  archivoDatos = SD.open("datos.csv", FILE_WRITE);

  if (archivoDatos)
  {

    archivoDatos.println("Fecha,Hora,Aire,Gas,Suelo,Temperatura,Humedad,Luz");

    archivoDatos.close();

    Serial.println("Cabecera CSV creada.");

  }

}


// ============================================================
// SETUP
// ============================================================

void setup()
{

  Serial.begin(115200);
  while (!Serial);

  Serial.println("Inicializando sistema ambiental IoT...");


  // ------------------------------------------------------------
  // BUS I2C
  // ------------------------------------------------------------

  Wire.begin();


  // ------------------------------------------------------------
  // OLED
  // ------------------------------------------------------------

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D))
  {
    Serial.println("Error OLED");
    while(1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(10,20);
  display.println("SysOnChip");

  display.setCursor(10,35);
  display.println("Sistema IoT");

  display.display();

  delay(2000);
  display.clearDisplay();


  // ------------------------------------------------------------
  // SENSOR SI7021
  // ------------------------------------------------------------

  if (!sensorAmbiente.begin())
  {

    Serial.println("Error SI7021");
    while (1);

  }


  // ------------------------------------------------------------
  // SENSOR BH1750
  // ------------------------------------------------------------

  if (!sensorLuz.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  {

    Serial.println("Error BH1750");
    while (1);

  }


  // ------------------------------------------------------------
  // RTC
  // ------------------------------------------------------------

  if (!relojRTC.begin())
  {

    Serial.println("Error RTC");
    while (1);

  }

  if (relojRTC.lostPower())
  {

    Serial.println("RTC sin energia");

    relojRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));

  }


  // ------------------------------------------------------------
  // TARJETA SD
  // ------------------------------------------------------------

  Serial.print("Inicializando SD... ");

  if (!SD.begin(pinSD))
  {

    Serial.println("Error SD");
    while (1);

  }

  Serial.println("OK");


  if (!SD.exists("datos.csv"))
  {

    Serial.println("Creando archivo CSV...");
    crearCabeceraCSV();

  }


  // ------------------------------------------------------------
  // WIFI
  // ------------------------------------------------------------

  Serial.println("Conectando WiFi...");

  WiFi.begin(nombreRed, claveRed);

  while (WiFi.status() != WL_CONNECTED)
  {

    delay(1000);
    Serial.print(".");

  }

  Serial.println();
  Serial.println("WiFi conectado");

  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());

  Serial.println("Sistema listo");

}


// ============================================================
// LOOP
// ============================================================

void loop()
{

  // ------------------------------------------------------------
  // TIEMPO
  // ------------------------------------------------------------

  DateTime tiempoActual = relojRTC.now();


  // ------------------------------------------------------------
  // SENSORES ANALOGICOS
  // ------------------------------------------------------------

  int valorAire  = analogRead(pinSensorAire);
  int valorGas   = analogRead(pinSensorGas);
  int valorSuelo = analogRead(pinSensorSuelo);


  // ------------------------------------------------------------
  // SENSORES DIGITALES
  // ------------------------------------------------------------

  float temperatura = sensorAmbiente.readTemperature();
  float humedad     = sensorAmbiente.readHumidity();
  float luz         = sensorLuz.readLightLevel();


  // ------------------------------------------------------------
  // SERIAL
  // ------------------------------------------------------------

  Serial.println("======= NUEVA MEDICION =======");

  Serial.print("Fecha: ");
  Serial.print(tiempoActual.year());
  Serial.print("-");
  Serial.print(tiempoActual.month());
  Serial.print("-");
  Serial.println(tiempoActual.day());

  Serial.print("Hora: ");

  imprimirDosDigitos(tiempoActual.hour());
  Serial.print(":");
  imprimirDosDigitos(tiempoActual.minute());
  Serial.print(":");
  imprimirDosDigitos(tiempoActual.second());

  Serial.println();

  Serial.print("Aire: "); Serial.println(valorAire);
  Serial.print("Gas: "); Serial.println(valorGas);
  Serial.print("Suelo: "); Serial.println(valorSuelo);
  Serial.print("Temp: "); Serial.println(temperatura);
  Serial.print("Hum: "); Serial.println(humedad);
  Serial.print("Luz: "); Serial.println(luz);
  

  // ------------------------------------------------------------
  // OLED
  // ------------------------------------------------------------

  display.clearDisplay();

  display.setCursor(0,0);

  display.print("Aire: ");
  display.println(valorAire);

  display.print("Gas: ");
  display.println(valorGas);

  display.print("Hum. en suelo:");
  display.println(valorSuelo);

  display.print("Temperatura:");
  display.print(temperatura);
  display.println("C");

  display.print("Humedad:");
  display.print(humedad);
  display.println(" %");

  display.print("Luminosidad:");
  display.print(luz);
  display.println("lx");

  display.display();


  // ------------------------------------------------------------
  // GUARDAR EN SD
  // ------------------------------------------------------------

  archivoDatos = SD.open("datos.csv", FILE_WRITE);

  if (archivoDatos)
  {

    archivoDatos.print(tiempoActual.year()); archivoDatos.print("-");
    archivoDatos.print(tiempoActual.month()); archivoDatos.print("-");
    archivoDatos.print(tiempoActual.day()); archivoDatos.print(",");

    archivoDatos.print(tiempoActual.hour()); archivoDatos.print(":");
    archivoDatos.print(tiempoActual.minute()); archivoDatos.print(":");
    archivoDatos.print(tiempoActual.second()); archivoDatos.print(",");

    archivoDatos.print(valorAire); archivoDatos.print(",");
    archivoDatos.print(valorGas); archivoDatos.print(",");
    archivoDatos.print(valorSuelo); archivoDatos.print(",");
    archivoDatos.print(temperatura); archivoDatos.print(",");
    archivoDatos.print(humedad); archivoDatos.print(",");
    archivoDatos.println(luz);

    archivoDatos.close();

    Serial.println("Datos guardados en SD");

  }


  // ------------------------------------------------------------
  // CREAR JSON
  // ------------------------------------------------------------

  String datosJSON = "{";

  datosJSON += "\"aire\":" + String(valorAire) + ",";
  datosJSON += "\"gas\":" + String(valorGas) + ",";
  datosJSON += "\"suelo\":" + String(valorSuelo) + ",";
  datosJSON += "\"temperatura\":" + String(temperatura,1) + ",";
  datosJSON += "\"humedad\":" + String(humedad,1) + ",";
  datosJSON += "\"luz\":" + String(luz,1);

  datosJSON += "}";


  Serial.println("JSON enviado:");
  Serial.println(datosJSON);


  // ------------------------------------------------------------
  // ENVIO HTTP
  // ------------------------------------------------------------

  clienteHTTP.beginRequest();

  clienteHTTP.post("/datos");

  clienteHTTP.sendHeader("Content-Type", "application/json");
  clienteHTTP.sendHeader("Content-Length", datosJSON.length());

  clienteHTTP.beginBody();
  clienteHTTP.print(datosJSON);
  clienteHTTP.endRequest();


  int codigoRespuesta = clienteHTTP.responseStatusCode();
  String respuestaServidor = clienteHTTP.responseBody();

  Serial.print("HTTP: ");
  Serial.println(codigoRespuesta);

  Serial.println(respuestaServidor);


  Serial.println("==============================");

  delay(5000);

}
