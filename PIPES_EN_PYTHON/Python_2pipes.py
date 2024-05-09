import os
#pull en master y nueva branch ahi

# PIPES FOR PIXY
pipe1_name = "/tmp/pipe1"
pipe3_name = "/tmp/pipe3"

#1 y 3
pipe_file_read1 = open(pipe1_name, "r")
print("PIPE1 READY")
pipe_file_read3 = open(pipe3_name, "r")
print("PIPE3 READY")

print("Esperando a recibir")

while(1):
    for line in pipe_file_read1:
        recieve = line
        print("RECIBIMOS DESDE PIPE1:")
        print(recieve)
        #if(recieve == "1\n"):
            #red
            #pipe_file_write.write("1\n")
            #pipe_file_write.flush()
        recieve = 0
    for line in pipe_file_read3:
        recieve = line
        print("RECIBIMOS DESDE PIPE3:")
        print(recieve)
        #if(recieve == "1\n"):
            #red
        #    pipe_file_write.write("1\n")
        #    pipe_file_write.flush()
        recieve = 0

pipe_file_read1.close()
pipe_file_read3.close()
#pipe_file_write.close()
#pipe_file_distance.close()