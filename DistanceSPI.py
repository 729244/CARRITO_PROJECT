#Distance and SPI
import os

# Crear pipe con nombre
pipe5_name = "/tmp/pipe5"
pipe6_name = "/tmp/pipe6"

pipe_file_read5 = open(pipe5_name, "r")
pipe_file_read6 = open(pipe6_name, "r")

print("Esperando a recibir")

while(1):
    string1 = pipe_file_read5.read()
    string2 = pipe_file_read6.read()
    #leemos los strings

pipe_file_read5.close()
pipe_file_read6.close()