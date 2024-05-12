import dev
import os
import spidev

def mamada(Distancia):
    spi = spidev.SpiDev()
    spi.open(0, 0)  # Open SPI bus 0, device 0
    spi.max_speed_hz = 1000000  # Set SPI speed to 1MHz

    # Convert the distance value to bytes
    distance_bytes = [Distancia & 0xFF, (Distancia >> 8) & 0xFF]

    # Send the distance bytes over SPI
    spi.xfer2(distance_bytes)

    spi.close()  # Close the SPI connection

# Example usage
mamada(100)