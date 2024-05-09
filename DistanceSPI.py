#Distance and SPI
import time
import os

# Crear pipe con nombre
pipe5_name = "/tmp/pipe5"
pipe6_name = "/tmp/pipe6"

time.sleep(1)

pipe_file_read5 = open(pipe5_name, "r")
pipe_file_read6 = open(pipe6_name, "r")

print("Esperando a recibir")

while(1):
    for line in pipe_file_read5:
        recieve = line
        recieve.split("R")
        print("RECIBIMOS DESDE PIPE5:")
        print(recieve)
        recieve = 0
    for line in pipe_file_read6:
        recieve = line
        recieve.split("R")
        print("RECIBIMOS DESDE PIPE6:")
        print(recieve)
        recieve = 0

pipe_file_read5.close()
pipe_file_read6.close()