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

ou2_flag = 0
ou1_flag = 0

DATA_1 = []
DATA_2 = []
DATA_F = []

spi = spidev.SpiDev()

def processObj(line):
    parts = line.split(":")
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
    if(ou1_flag == 0):
        for line in pipe_file_read6:
            recieve = line
            if(counter1 == 0): #Significa que es el dato donde indican los objetos
                ou1_obj = int(recieve[0])
                print("Para out.ppm se esperan " + str(ou1_obj) + " objetos")
                if(ou1_obj != 0):
                    counter1 = counter1 + 1
                break
            print("RECIBIMOS DESDE out.ppm o PADRE:")
            [class1, xmin1, xprom1, xmax1, ymin1] = processObj(recieve)
            DATA_1[line] = [class1, xmin1, xprom1, xmax1, ymin1]
            counter1 = counter1 + 1
            if(counter1 == ou1_obj+1):
                ou1_flag = 1
                counter1 = 0
            break

    if(ou2_flag == 0):
        for line2 in pipe_file_read5:
            recieve2 = line2
            if(counter2 == 0): #Significa que es el dato donde indican los objetos
                ou2_obj = int(recieve2[0])
                print("Para out2.ppm se esperan " + str(ou2_obj) + " objetos")
                if(ou2_obj != 0):
                    counter2 = counter2 + 1
                break
            print("RECIBIMOS DESDE out2.ppm o HIJO:")
            #Falta poner los objetos que recibe en lo que se te antoje y despues ya lo de la distancia
            [class2, xmin2, xprom2,xmax2, ymin2] = processObj(recieve2)
            DATA_2[line2] = [class2, xmin2, xprom2, xmax2, ymin2]
            counter2 = counter2 + 1
            if(counter2 == ou2_obj+1):
                ou2_flag = 1
                counter2 = 0
            break

    if(ou1_flag == 1 and ou2_flag == 1):
        #data ready to tal
        #Compare object numbers and classes and the ones who are the same, get objClass,xmin,xprom,xmax and distance to send it in SPI
        #First send number of valid objects
        #Then send the array of length 5*ObjNums each one one byte and meaning the description above with some enumerated type data being
        #CLASS_NUM
        #For xmin,xprom and xmax LEFT(0),CENTER(1),RIGHT(2)
        #For Distance CLOSE(0),MID(1),FAR(2)
        if(len(DATA_1) == len(DATA_2)):
            spi.open(0, 0)
            spi.xfer([len(DATA_1)])
            spi.close()
            for i in range(len(DATA_1)):
                xclass1 = DATA_1[i][0]
                xclass2 = DATA_2[i][0]
                xprom1 = DATA_1[i][2]
                xprom2 = DATA_2[i][2]
                result_f = (xprom1 / xprom2) * 7
                if (xclass1 == xclass2):
                    spi.open(0, 0)
                    DATA_F.append(xclass1)
                    DATA_F.append(result_f)
                    for i in range(len(DATA_F)):
                        size = len(str(DATA_F[i]).encode())
                        spi.xfer([size])
                        spi.xfer([DATA_F[i]])
                        spi.close()
        else:
            print("No se pueden comparar los objetos")
            spi.open(0, 0)
            spi.xfer(200)
            spi.close()

        ou2_flag = 0 
        ou1_flag = 0



pipe_file_read5.close()
pipe_file_read6.close()