#ifndef __GPS_h

#include <stdint.h>
#include <stdbool.h>

// Estructura para almacenar los datos decodificados
typedef struct {
    double latitud;
    uint8_t latitud_dir;
    double longitud;
    uint8_t longitud_dir;
    double velocidad;
} GPSData;

void decodeGPRMC(uint8_t*, GPSData*);
void decodeGPGGA(uint8_t*, GPSData*);
bool verifyChecksum(uint8_t*);


#endif