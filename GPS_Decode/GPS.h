#ifndef __GPS_h

#include <stdint.h>

// Estructura para almacenar los datos decodificados
typedef struct {
    double latitud;
    uint8_t latitud_dir;
    double longitud;
    uint8_t longitud_dir;
    double velocidad;
} GPSData;



#endif