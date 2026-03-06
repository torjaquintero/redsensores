/*
 *  Bienvenidos a Sys On Chip
 * ==============================
 * Ejercicio No.3
 *
 * En este programa vamos a integrar un módulo de reloj en tiempo real
 * (RTC DS3231) con una estación ambiental basada en Arduino.
 *
 * El sistema leerá diferentes sensores:
 *
 *  - MQ135  -> Calidad del aire
 *  - MQ9    -> Gases combustibles
 *  - Sensor de humedad del suelo
 *  - SI7021 -> Temperatura y humedad ambiente
 *  - BH1750 -> Iluminación
 *
 * Cada lectura de sensores será registrada junto con
 * la fecha y hora actual proporcionada por el RTC.
 *
 * Esto introduce el concepto de:
 *
 *  - Timestamp
 *  - Registro temporal de datos
 *  - Sistemas de monitoreo ambiental
 *
 * ==============================================================================================
 */


// ============================================================
// Declaración de librerías
// ============================================================

#include <Wire.h>
#include <Adafruit_Si7021.h>
#include <BH1750.h>
#include "RTClib.h"


// ============================================================
// Definición de pines analógicos
// ============================================================

const int pinSensorAire = A0;
const int pinSensorGas = A1;
const int pinSensorSuelo = A2;


// ============================================================
// Creación de objetos de sensores
// ============================================================

Adafruit_Si7021 sensorAmbiente = Adafruit_Si7021();
BH1750 sensorLuz;
RTC_DS3231 relojRTC;


// ============================================================
// Arreglos de días y meses
// ============================================================

String diasSemana[7] = {
  "Domingo","Lunes","Martes","Miercoles",
  "Jueves","Viernes","Sabado"
};

String mesesAnio[12] = {
  "Enero","Febrero","Marzo","Abril","Mayo","Junio",
  "Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre"
};


// ============================================================
// Función para imprimir números con dos dígitos
// Ejemplo: 01, 02, 09
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
// Función para mostrar fecha y hora
// ============================================================

void mostrarFechaHora(DateTime tiempoActual)
{

  Serial.print("Fecha: ");

  Serial.print(tiempoActual.day());
  Serial.print(" de ");
  Serial.print(mesesAnio[tiempoActual.month() - 1]);
  Serial.print(" de ");
  Serial.print(tiempoActual.year());

  Serial.print(" (");
  Serial.print(diasSemana[tiempoActual.dayOfTheWeek()]);
  Serial.print(")");

  Serial.print("  |  Hora: ");

  imprimirDosDigitos(tiempoActual.hour());
  Serial.print(":");

  imprimirDosDigitos(tiempoActual.minute());
  Serial.print(":");

  imprimirDosDigitos(tiempoActual.second());

  Serial.println();
}


// ============================================================
// Configuración inicial
// ============================================================

void setup()
{

  Serial.begin(115200);

  while (!Serial);

  Serial.println("Inicializando sistema de monitoreo ambiental...");
  Serial.println();


  // Inicializar comunicación I2C
  Wire.begin();


  // ------------------------------------------------------------
  // Inicializar sensor de temperatura y humedad
  // ------------------------------------------------------------

  if (!sensorAmbiente.begin())
  {
    Serial.println("Error: Sensor SI7021 no encontrado");
    while (1);
  }


  // ------------------------------------------------------------
  // Inicializar sensor de iluminación
  // ------------------------------------------------------------

  if (!sensorLuz.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  {
    Serial.println("Error: Sensor BH1750 no encontrado");
    while (1);
  }


  // ------------------------------------------------------------
  // Inicializar reloj RTC
  // ------------------------------------------------------------

  if (!relojRTC.begin())
  {
    Serial.println("Error: No se detecta el modulo RTC");
    while (1);
  }


  // Si el reloj perdió energía
  if (relojRTC.lostPower())
  {
    Serial.println("RTC sin energia, ajustando fecha y hora...");
    relojRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.println("Sistema inicializado correctamente");
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

  int valorSensorAire = analogRead(pinSensorAire);
  int valorSensorGas = analogRead(pinSensorGas);
  int valorHumedadSuelo = analogRead(pinSensorSuelo);


  // ------------------------------------------------------------
  // Lectura de sensores digitales
  // ------------------------------------------------------------

  float temperaturaAmbiente = sensorAmbiente.readTemperature();
  float humedadAmbiente = sensorAmbiente.readHumidity();
  float iluminancia = sensorLuz.readLightLevel();


  // ------------------------------------------------------------
  // Mostrar datos en el monitor serial
  // ------------------------------------------------------------

  Serial.println("========= REGISTRO DE SENSORES =========");

  // Mostrar fecha y hora de la medición
  mostrarFechaHora(tiempoActual);

  Serial.println();

  Serial.print("MQ135 - Calidad del aire: ");
  Serial.println(valorSensorAire);

  Serial.print("MQ9 - Gas CO / GLP: ");
  Serial.println(valorSensorGas);

  Serial.print("Humedad del suelo: ");
  Serial.println(valorHumedadSuelo);

  Serial.print("Temperatura ambiente (C): ");
  Serial.println(temperaturaAmbiente);

  Serial.print("Humedad ambiente (%): ");
  Serial.println(humedadAmbiente);

  Serial.print("Iluminacion (lux): ");
  Serial.println(iluminancia);

  Serial.println("=========================================");
  Serial.println();


  // Esperar 2 segundos
  delay(2000);
}
