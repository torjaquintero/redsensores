/*
 *  Bienvenidos a Sys On Chip
 * ==============================
 * Ejercicio No.4
 *
 * En este programa vamos a construir un sistema de adquisición de datos
 * ambientales utilizando diferentes sensores conectados al Arduino.
 *
 * El sistema leerá los siguientes sensores:
 *
 *  - Sensor de calidad del aire MQ135
 *  - Sensor de gas MQ9
 *  - Sensor de humedad del suelo
 *  - Sensor de temperatura y humedad SI7021
 *  - Sensor de luz BH1750
 *
 * Además utilizaremos un módulo RTC DS3231 para obtener la fecha y hora
 * exacta de cada medición.
 *
 * Finalmente todos los datos se almacenarán en una tarjeta microSD
 * en un archivo con formato CSV para poder analizarlos posteriormente
 * en programas como Excel, Python o MATLAB.
 *
 * El sistema verifica si el archivo CSV ya existe:
 *
 *  - Si el archivo NO existe → crea el archivo y escribe la cabecera.
 *  - Si el archivo YA existe → continúa agregando nuevos datos.
 *
 * ==============================================================================================
 */


// ============================================================
// Declaración de librerías
// ============================================================

#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include <Adafruit_Si7021.h>
#include <BH1750.h>
#include "RTClib.h"


// ============================================================
// Definición de pines analógicos
// ============================================================

const int pinSensorAire  = A0;
const int pinSensorGas   = A1;
const int pinSensorSuelo = A2;


// Pin Chip Select del módulo microSD
const int pinSD = 10;


// ============================================================
// Creación de objetos de sensores
// ============================================================

Adafruit_Si7021 sensorAmbiente = Adafruit_Si7021();
BH1750 sensorLuz;
RTC_DS3231 relojRTC;

File archivoDatos;


// ============================================================
// Función para imprimir números con dos dígitos
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
// Función que crea la cabecera del archivo CSV
// ============================================================

void crearCabeceraCSV()
{

  archivoDatos = SD.open("datos.csv", FILE_WRITE);

  if (archivoDatos)
  {

    archivoDatos.println("Fecha,Hora,Aire,Gas,HumedadSuelo,Temperatura,Humedad,Luz");

    archivoDatos.close();

    Serial.println("Cabecera del archivo CSV creada.");

  }

}


// ============================================================
// Configuración inicial del sistema
// ============================================================

void setup()
{

  Serial.begin(115200);
  while (!Serial);

  Serial.println("Inicializando sistema de monitoreo ambiental...");
  Serial.println();


  // ------------------------------------------------------------
  // Inicializar bus I2C
  // ------------------------------------------------------------

  Wire.begin();


  // ------------------------------------------------------------
  // Inicializar sensor SI7021
  // ------------------------------------------------------------

  if (!sensorAmbiente.begin())
  {
    Serial.println("Error al detectar el sensor SI7021");
    while (1);
  }


  // ------------------------------------------------------------
  // Inicializar sensor BH1750
  // ------------------------------------------------------------

  if (!sensorLuz.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  {
    Serial.println("Error al detectar el sensor BH1750");
    while (1);
  }


  // ------------------------------------------------------------
  // Inicializar reloj RTC
  // ------------------------------------------------------------

  if (!relojRTC.begin())
  {
    Serial.println("Error al detectar el modulo RTC");
    while (1);
  }

  if (relojRTC.lostPower())
  {
    Serial.println("El RTC perdió energía, ajustando fecha y hora");
    relojRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }


  // ------------------------------------------------------------
  // Inicializar tarjeta microSD
  // ------------------------------------------------------------

  Serial.print("Inicializando tarjeta microSD... ");

  if (!SD.begin(pinSD))
  {
    Serial.println("Error al inicializar la tarjeta SD");
    while (1);
  }

  Serial.println("Tarjeta SD lista");


  // ------------------------------------------------------------
  // Verificar si el archivo ya existe
  // ------------------------------------------------------------

  if (!SD.exists("datos.csv"))
  {

    Serial.println("Archivo datos.csv no existe, creando archivo...");
    crearCabeceraCSV();

  }

  else
  {

    Serial.println("Archivo datos.csv encontrado.");
    Serial.println("Se continuarán agregando datos.");

  }


  Serial.println();
  Serial.println("Sistema listo para registrar datos");
  Serial.println();

}


// ============================================================
// Bucle principal
// ============================================================

void loop()
{

  // ------------------------------------------------------------
  // Obtener fecha y hora actual
  // ------------------------------------------------------------

  DateTime tiempoActual = relojRTC.now();


  // ------------------------------------------------------------
  // Lectura de sensores analógicos
  // ------------------------------------------------------------

  int valorAire  = analogRead(pinSensorAire);
  int valorGas   = analogRead(pinSensorGas);
  int valorSuelo = analogRead(pinSensorSuelo);


  // ------------------------------------------------------------
  // Lectura de sensores digitales
  // ------------------------------------------------------------

  float temperatura = sensorAmbiente.readTemperature();
  float humedad     = sensorAmbiente.readHumidity();
  float luz         = sensorLuz.readLightLevel();


  // ------------------------------------------------------------
  // Mostrar datos en el monitor serial
  // ------------------------------------------------------------

  Serial.println("======= NUEVO REGISTRO =======");

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
  Serial.println();


  Serial.print("MQ135 - Calidad del aire: ");
  Serial.println(valorAire);

  Serial.print("MQ9 - Gas CO / GLP: ");
  Serial.println(valorGas);

  Serial.print("Humedad del suelo: ");
  Serial.println(valorSuelo);

  Serial.print("Temperatura ambiente (C): ");
  Serial.println(temperatura);

  Serial.print("Humedad ambiente (%): ");
  Serial.println(humedad);

  Serial.print("Iluminacion (lux): ");
  Serial.println(luz);

  Serial.println();


  // ------------------------------------------------------------
  // Guardar datos en microSD
  // ------------------------------------------------------------

  archivoDatos = SD.open("datos.csv", FILE_WRITE);

  if (archivoDatos)
  {

    archivoDatos.print(tiempoActual.year());
    archivoDatos.print("-");
    archivoDatos.print(tiempoActual.month());
    archivoDatos.print("-");
    archivoDatos.print(tiempoActual.day());
    archivoDatos.print(",");


    if (tiempoActual.hour() < 10) archivoDatos.print("0");
    archivoDatos.print(tiempoActual.hour());
    archivoDatos.print(":");

    if (tiempoActual.minute() < 10) archivoDatos.print("0");
    archivoDatos.print(tiempoActual.minute());
    archivoDatos.print(":");

    if (tiempoActual.second() < 10) archivoDatos.print("0");
    archivoDatos.print(tiempoActual.second());

    archivoDatos.print(",");


    archivoDatos.print(valorAire);
    archivoDatos.print(",");

    archivoDatos.print(valorGas);
    archivoDatos.print(",");

    archivoDatos.print(valorSuelo);
    archivoDatos.print(",");

    archivoDatos.print(temperatura);
    archivoDatos.print(",");

    archivoDatos.print(humedad);
    archivoDatos.print(",");

    archivoDatos.println(luz);

    archivoDatos.close();

    Serial.println("Datos guardados en memoria microSD");

  }

  else
  {

    Serial.println("Error al abrir el archivo en la tarjeta SD");

  }


  Serial.println("==============================");
  Serial.println();


  // Espera entre mediciones
  delay(2000);

}
