#Distance and SPI
import time
import os

# Crear pipe con nombre
pipe5_name = "/tmp/pipe5"
pipe6_name = "/tmp/pipe6"

time.sleep(1)

pipe_file_read5 = open(pipe5_name, "r")
pipe_file_read6 = open(pipe6_name, "r")

counter2 = 0
counter1 = 0
ou2_obj = 0
ou1_obj = 0



def processObj(line):
    parts = line.split(":")
    print(parts)
    clase = parts[1].replace("', {","").replace("}, '","")
    print("Clase: " + clase)
    xmin = float(parts[2].replace("', {","").replace("}, '",""))
    print("Xmin: " + str(xmin))
    xprom = float(parts[3].replace("', {","").replace("}, '",""))
    print("Xprom: " + str(xprom))
    xmax = float(parts[4].replace("', {","").replace("}, '",""))
    print("Xmax: " + str(xmax))
    ymin =float(parts[5].replace("', {","").replace("}, '",""))
    print("Ymin: " + str(ymin))
    return clase,xmin,xprom,xmax,ymin


while(1):
    for line2 in pipe_file_read5:
        recieve2 = line2
        if(counter2 == 0): #Significa que es el dato donde indican los objetos
            ou2_obj = int(recieve2[0])
            print("Para out2.ppm se esperan " + str(ou2_obj) + " objetos")
            counter2 = counter2 + 1
            break
        print("RECIBIMOS DESDE out2.ppm o HIJO:")
        #Falta poner los objetos que recibe en lo que se te antoje y despues ya lo de la distancia
        processObj(recieve2)
        counter2 = counter2 + 1
        if(counter2 == ou2_obj+1):
            counter2 = 0
        break
    for line in pipe_file_read6:
        recieve = line
        if(counter1 == 0): #Significa que es el dato donde indican los objetos
            ou1_obj = int(recieve[0])
            print("Para out.ppm se esperan " + str(ou1_obj) + " objetos")
            counter1 = counter1 + 1
            break
        print("RECIBIMOS DESDE out.ppm o PADRE:")
        processObj(recieve)
        counter1 = counter1 + 1
        if(counter1 == ou1_obj+1):
            counter1 = 0
        break

pipe_file_read5.close()
pipe_file_read6.close()