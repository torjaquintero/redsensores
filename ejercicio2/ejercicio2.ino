/*
 *  Bienvenidos a Sys On Chip
 * ==============================
 * Ejercicio No.2
 *
 * En este programa vamos a utilizar un módulo de reloj en tiempo real
 * basado en el circuito integrado DS3231.
 *
 * Un RTC (Real Time Clock) permite mantener la fecha y la hora incluso
 * cuando el sistema se apaga gracias a una batería interna.
 *
 * El módulo se comunica con Arduino mediante el protocolo I2C.
 *
 * El sistema mostrará la fecha y hora actual cada segundo
 * con formato de hora: HH:MM:SS
 *
 * Ejemplo:
 * 12:01:01
 *
 * ==============================================================================================
 */


// ============================================================
// Declaración de librerías
// ============================================================

#include <Wire.h>
#include "RTClib.h"


// ============================================================
// Creación del objeto del reloj
// ============================================================

RTC_DS3231 relojRTC;


// ============================================================
// Arreglos con días y meses
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
// Ejemplo:
// 1  -> 01
// 9  -> 09
// 12 -> 12
// ============================================================

void imprimirDosDigitos(int numero)
{
  if(numero < 10)
  {
    Serial.print("0");
  }

  Serial.print(numero);
}


// ============================================================
// Configuración inicial
// ============================================================

void setup()
{
  Serial.begin(115200);

  while (!Serial);

  Serial.println("Inicializando modulo RTC DS3231...");
  Serial.println();

  // Inicializar bus I2C
  Wire.begin();


  // Verificar conexión con el RTC
  if (!relojRTC.begin())
  {
    Serial.println("Error: No se detecta el modulo RTC");
    while (1);
  }


  // Si el reloj perdió energía
  if (relojRTC.lostPower())
  {
    Serial.println("El RTC perdio la alimentacion.");
    Serial.println("Ajustando fecha y hora con la compilacion.");

    // Ajustar con fecha y hora del computador
    relojRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.println("RTC inicializado correctamente");
  Serial.println();
}


// ============================================================
// Función para mostrar fecha y hora
// ============================================================

void mostrarFechaHora(DateTime tiempoActual)
{

  Serial.println("====== FECHA Y HORA ACTUAL ======");

  // Mostrar fecha
  Serial.print("Fecha: ");

  Serial.print(tiempoActual.day());
  Serial.print(" de ");

  Serial.print(mesesAnio[tiempoActual.month()-1]);
  Serial.print(" de ");

  Serial.print(tiempoActual.year());

  Serial.print(" (");
  Serial.print(diasSemana[tiempoActual.dayOfTheWeek()]);
  Serial.println(")");



  // Mostrar hora con formato HH:MM:SS
  Serial.print("Hora: ");

  imprimirDosDigitos(tiempoActual.hour());
  Serial.print(":");

  imprimirDosDigitos(tiempoActual.minute());
  Serial.print(":");

  imprimirDosDigitos(tiempoActual.second());

  Serial.println();
  Serial.println("===============================");
  Serial.println();
}


// ============================================================
// Bucle principal
// ============================================================

void loop()
{

  // Obtener fecha y hora actual
  DateTime tiempoActual = relojRTC.now();

  // Mostrar datos en serial
  mostrarFechaHora(tiempoActual);

  // Esperar 1 segundo
  delay(1000);
}
