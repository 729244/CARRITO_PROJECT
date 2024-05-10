import os
#pull en master y nueva branch ahi

# PIPES FOR PIXY
pipe1_name = "/tmp/pipe1"
pipe2_name = "/tmp/pipe2"
pipe3_name = "/tmp/pipe3"
pipe4_name = "/tmp/pipe4"

#1 y 3
pipe_file_read1 = open(pipe1_name, "r")
print("PIPE1 READY")
pipe_file_write2 = open(pipe2_name, "w")
print("PIPE2 READY")
pipe_file_read3 = open(pipe3_name, "r")
print("PIPE3 READY")
pipe_file_write4 = open(pipe4_name, "w")
print("PIPE4 READY")

print("Esperando a recibir")

while(1):
    for line in pipe_file_read1:
        recieve = line
        print("RECIBIMOS DESDE PIPE1:")
        print(recieve)
        if(recieve == "1\n"):
            #red
            print("Solicitando nuevo out.ppm")
            pipe_file_write2.write("1\n")
            pipe_file_write2.flush()
        recieve = 0
        break
    for line2 in pipe_file_read3:
        recieve2 = line2
        print("RECIBIMOS DESDE PIPE3:")
        print(recieve2)
        if(recieve2 == "1\n"):
            #red
            print("Solicitando nuevo out2.ppm")
            pipe_file_write4.write("1\n")
            pipe_file_write4.flush()
        recieve2 = 0
        break

pipe_file_read1.close()
pipe_file_read3.close()
pipe_file_read2.close()
pipe_file_read4.close()
#pipe_file_write.close()
#pipe_file_distance.close()