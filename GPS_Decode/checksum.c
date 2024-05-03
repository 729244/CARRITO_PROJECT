bool verifyChecksum(const char* sentence) {
    // Busca el asterisco en la trama
    const char* asterisk = sentence;
    while (*asterisk && *asterisk != '*') {
        ++asterisk;
    }
    if (!*asterisk) {
        printf("Trama inválida (sin asterisco).\n");
        return false;
    }

    // Calcula el checksum esperado
    unsigned char expectedChecksum = 0;
    for (const char* ptr = sentence + 1; ptr < asterisk; ++ptr) {
        if (*ptr == '$') {
            continue; // Ignora el signo de dólar
        }
        expectedChecksum ^= hexCharToInt(*ptr);
    }

    // Obtiene el checksum proporcionado en la trama
    unsigned char providedChecksum;
    ++asterisk; // Avanza al siguiente carácter después del asterisco
    if (isxdigit(asterisk[0]) && isxdigit(asterisk[1])) {
        providedChecksum = (hexCharToInt(asterisk[0]) << 4) | hexCharToInt(asterisk[1]);
    } else {
        printf("Trama inválida (checksum incorrecto).\n");
        return false;
    }

    // Compara los checksums
    if (expectedChecksum == providedChecksum) {
        printf("Checksum válido.\n");
        return true;
    } else {
        printf("Checksum inválido (esperado: %02X, proporcionado: %02X).\n", expectedChecksum, providedChecksum);
        return false;
    }
}
