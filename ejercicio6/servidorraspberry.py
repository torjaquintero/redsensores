"""
Bienvenidos a Sys On Chip
==============================

Servidor de adquisición de datos para Raspberry Pi

Este programa recibe datos enviados desde un Arduino
mediante HTTP en formato JSON y los guarda en un archivo CSV.
"""

# ============================================================
# Librerías
# ============================================================

from flask import Flask, request, jsonify
import csv
from datetime import datetime
import os


# ============================================================
# Crear aplicación Flask
# ============================================================

app = Flask(__name__)

archivo_csv = "datos_raspberry.csv"


# ============================================================
# Crear cabecera del CSV si no existe
# ============================================================

def crear_archivo_csv():

    if not os.path.exists(archivo_csv):

        with open(archivo_csv, "w", newline="") as archivo:

            escritor = csv.writer(archivo)

            escritor.writerow([
                "Fecha",
                "Hora",
                "Aire",
                "Gas",
                "Temperatura",
                "Humedad",
                "Luz"
            ])

        print("Archivo CSV creado")


# ============================================================
# Ruta que recibe datos del Arduino
# ============================================================

@app.route("/datos", methods=["POST"])
def recibir_datos():

    try:

        datos = request.get_json()

        if datos is None:
            return jsonify({"error": "No se recibio JSON"}), 400

        print("Datos recibidos:")
        print(datos)

        # Obtener fecha y hora

        ahora = datetime.now()

        fecha = ahora.strftime("%Y-%m-%d")
        hora = ahora.strftime("%H:%M:%S")

        # Extraer variables

        aire = datos.get("aire")
        gas = datos.get("gas")
        temperatura = datos.get("temperatura")
        humedad = datos.get("humedad")
        luz = datos.get("luz")

        # Guardar en CSV

        with open(archivo_csv, "a", newline="") as archivo:

            escritor = csv.writer(archivo)

            escritor.writerow([
                fecha,
                hora,
                aire,
                gas,
                temperatura,
                humedad,
                luz
            ])

        print("Datos guardados correctamente")
        print()

        return jsonify({"mensaje": "Datos almacenados"}), 200

    except Exception as e:

        print("Error:", e)
        return jsonify({"error": str(e)}), 500


# ============================================================
# Inicio del servidor
# ============================================================

if __name__ == "__main__":

    crear_archivo_csv()

    print("===================================")
    print("Servidor SysOnChip iniciado")
    print("Esperando datos desde Arduino...")
    print("Puerto: 5000")
    print("===================================")

    app.run(host="0.0.0.0", port=5000)
