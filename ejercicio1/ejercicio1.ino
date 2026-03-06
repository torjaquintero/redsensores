/* 
 *  Bienvenidos a Sys On Chip
 * ==============================
 * Ejercicio No.1
 *
 * En este programa vamos a construir una pequeña estación ambiental
 * utilizando diferentes sensores conectados a un Arduino UNO R4 WiFi.
 *
 * Sensores utilizados:
 *
 *  - MQ135  -> Sensor de calidad del aire (entrada analógica)
 *  - MQ9    -> Sensor de gases combustibles CO / GLP (entrada analógica)
 *  - Higrómetro de suelo -> Sensor de humedad del suelo (entrada analógica)
 *  - SI7021 -> Sensor de temperatura y humedad ambiental (I2C)
 *  - BH1750 -> Sensor digital de iluminación (I2C)
 *
 * El sistema realizará la lectura de todos los sensores cada 2 segundos
 * y mostrará los resultados en el monitor serial.
 *
 * Este ejercicio introduce conceptos fundamentales de:
 *
 *  - Lectura de entradas analógicas
 *  - Comunicación I2C
 *  - Integración de múltiples sensores
 *  - Monitoreo ambiental con Arduino
 *
 * ==============================================================================================
 */

#include <Wire.h>              // Librería para comunicación I2C
#include <Adafruit_Si7021.h>   // Librería para el sensor de temperatura y humedad
#include <BH1750.h>            // Librería para el sensor de iluminación


/* ============================================================
   Definición de pines analógicos
   ============================================================ */

// Sensor de calidad del aire
const int pinSensorAire = A0;

// Sensor de gases combustibles
const int pinSensorGas = A1;

// Sensor de humedad del suelo
const int pinSensorSuelo = A2;


/* ============================================================
   Creación de objetos de sensores I2C
   ============================================================ */

// Objeto para el sensor de temperatura y humedad
Adafruit_Si7021 sensorAmbiente = Adafruit_Si7021();

// Objeto para el sensor de iluminación
BH1750 sensorLuz;


/* ============================================================
   Configuración inicial del sistema
   ============================================================ */

void setup() {

  // Inicializar comunicación serial
  // Esto permite visualizar los datos en el monitor serial
  Serial.begin(115200);

  // Esperar a que el puerto serial esté listo
  while (!Serial);

  Serial.println("Inicializando sensores...");
  Serial.println();

  // Inicializar el bus I2C
  Wire.begin();


  /* ------------------------------------------------------------
     Inicialización del sensor de temperatura y humedad
     ------------------------------------------------------------ */

  if (!sensorAmbiente.begin()) {

    Serial.println("Error: Sensor SI7021 no encontrado.");
    Serial.println("Verifique las conexiones I2C.");

    // Detener el programa si el sensor no se detecta
    while (1);
  }
  else {

    Serial.println("Sensor SI7021 inicializado correctamente.");
  }


  /* ------------------------------------------------------------
     Inicialización del sensor de iluminación
     ------------------------------------------------------------ */

  if (sensorLuz.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {

    Serial.println("Sensor BH1750 inicializado correctamente.");
  }
  else {

    Serial.println("Error: Sensor BH1750 no encontrado.");
    Serial.println("Verifique las conexiones I2C.");

    // Detener el programa si el sensor no se detecta
    while (1);
  }

  Serial.println();
  Serial.println("Sistema listo.");
  Serial.println();
}


/* ============================================================
   Bucle principal del programa
   ============================================================ */

void loop() {

  /* ------------------------------------------------------------
     Lectura de sensores analógicos
     ------------------------------------------------------------ */

  // Leer el valor del sensor de calidad del aire
  int valorSensorAire = analogRead(pinSensorAire);

  // Leer el valor del sensor de gases combustibles
  int valorSensorGas = analogRead(pinSensorGas);

  // Leer el valor del sensor de humedad del suelo
  int valorHumedadSuelo = analogRead(pinSensorSuelo);


  /* ------------------------------------------------------------
     Lectura de sensores digitales (I2C)
     ------------------------------------------------------------ */

  // Leer humedad relativa del ambiente
  float humedadAmbiente = sensorAmbiente.readHumidity();

  // Leer temperatura ambiente
  float temperaturaAmbiente = sensorAmbiente.readTemperature();

  // Leer nivel de iluminación en lux
  float iluminancia = sensorLuz.readLightLevel();


  /* ------------------------------------------------------------
     Mostrar datos en el monitor serial
     ------------------------------------------------------------ */

  Serial.println("========= LECTURA DE SENSORES =========");

  Serial.print("Sensor MQ135 (calidad del aire): ");
  Serial.println(valorSensorAire);

  Serial.print("Sensor MQ9 (gas CO / GLP): ");
  Serial.println(valorSensorGas);

  Serial.print("Humedad del suelo: ");
  Serial.println(valorHumedadSuelo);

  Serial.print("Temperatura ambiente (C): ");
  Serial.println(temperaturaAmbiente);

  Serial.print("Humedad relativa ambiente (%): ");
  Serial.println(humedadAmbiente);

  Serial.print("Nivel de iluminacion (lux): ");
  Serial.println(iluminancia);

  Serial.println("=======================================");
  Serial.println();


  /* ------------------------------------------------------------
     Esperar 2 segundos antes de la siguiente lectura
     ------------------------------------------------------------ */

  delay(2000);
}
