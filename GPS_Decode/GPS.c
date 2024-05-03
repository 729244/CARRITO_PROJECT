#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Estructura para almacenar los datos decodificados
typedef struct {
    float latitud;
    uint8_t latitud_dir;
    float longitud;
    uint8_t longitud_dir;
    float velocidad;
} GPSData;

// Función para extraer un campo específico de la trama
char* getField(const char* sentence, int fieldIndex) {
    char* token = strtok((char*)sentence, ",");
    for (int i = 0; i < fieldIndex; ++i) {
        token = strtok(NULL, ",");
    }
    return token;
}

// Función para decodificar la trama GPRMC
void decodeGPRMC(const char* sentence, GPSData* gpsData) {
    char status = getField(sentence, 2)[0];
    double lat_deg = atof(getField(sentence, 3));
    char lat_dir = getField(sentence, 4)[0];
    double lon_deg = atof(getField(sentence, 5));
    char lon_dir = getField(sentence, 6)[0];

    // Verificamos si la posición es válida
    if (status == 'A') {
        gpsData->latitud = lat_deg / 100.0; // Convertimos de grados y minutos a decimal
        gpsData->longitud = lon_deg / 100.0;
        gpsData->latitud_dir = lat_dir;
        gpsData->longitud_dir = lon_dir;
        gpsData->velocidad = atof(getField(sentence, 7));
    } else {
        printf("Posición no válida.\n");
    }
}

void decodeGPGGA(const char* sentence, GPSData* gpsData) {
    gpsData->latitud = atof(getField(sentence, 2));
    gpsData->latitud_dir = getField(sentence, 3)[0];
    gpsData->longitud = atof(getField(sentence, 4));
    gpsData->longitud_dir = getField(sentence, 5)[0];
    //gpsData->calidad = atoi(getField(sentence, 6));
    //gpsData->nsat = atoi(getField(sentence, 7));
    //gpsData->hdop = atof(getField(sentence, 8));
    //gpsData->altitud = atof(getField(sentence, 9));
}