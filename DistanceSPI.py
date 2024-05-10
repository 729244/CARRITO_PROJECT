#Distance and SPI
import time
import os

# Crear pipe con nombre
pipe5_name = "/tmp/pipe5"
pipe6_name = "/tmp/pipe6"

time.sleep(1)

pipe_file_read5 = open(pipe5_name, "r")
pipe_file_read6 = open(pipe6_name, "r")

while(1):
    for line2 in pipe_file_read5:
        recieve2 = line2
        recieve2.split("R")
        print("RECIBIMOS DESDE out2.ppm o HIJO:")
        print(recieve2)
        break
    for line in pipe_file_read6:
        recieve = line
        recieve.split("R")
        print("RECIBIMOS DESDE out.ppm o PADRE:")
        print(recieve)
        break
    for c in range(len(recieve)):
        if ((recieve[c] == "R:") or (recieve[c] == ":")):
            recieve[c].pop()
    for c2 in range(len(recieve2)):
        if ((recieve2[c2] == "R:") or (recieve2[c2] == ":")):
            recieve2[c2].pop()
    print(recieve)
    print(recieve2)
    recieve = 0
    recieve2 = 0

pipe_file_read5.close()
pipe_file_read6.close()