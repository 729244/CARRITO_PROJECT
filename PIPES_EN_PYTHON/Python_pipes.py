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

print("Esperando a recibir")

while(1):
    for line in pipe_file_read:
        recieve = line
        if(recieve == "1\n"):
            #red
            pipe_file_write.write("1\n")
            pipe_file_write.flush()
            recieve = 0
pipe_file_read.close()
pipe_file_write.close()