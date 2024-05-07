import os
#pull en master y nueva branch ahi

# Crear pipe con nombre
pipe1_name = "/tmp/pipe1"
pipe2_name = "/tmp/pipe2"
pipe3_name = "/tmp/pipe3"
pipe4_name = "/tmp/pipe4"

p = os.fork()

if(p == 0):
    #3 y 4
    pipe_file_read = open(pipe3_name, "r")
    pipe_file_write = open(pipe4_name, "w")
else:
    #1 y 2
    pipe_file_read = open(pipe1_name, "r")
    pipe_file_write = open(pipe2_name, "w")

while(1):
    recieve = pipe_file_read.read()
    if(recieve == 49):
        if(p == 0):
            print("Se recibió correctamente desde el proceso hijo")
        else:
            print("Se recibió correctamente desde el proceso padre")
    else:
        if(p == 0):
            print("No se recibió correctamente el dato, soy el proceso hijo")
        else:
            print("No se recibió correctamente el dato, soy el proceso padre")
    #hacer proceso de la red
    pipe_file_write.write("1")

pipe_file_read.close()
pipe_file_write.close()