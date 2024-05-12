#Distance and SPI
import time
import os

# Crear pipe con nombre
pipe5_name = "/tmp/pipe5"
pipe6_name = "/tmp/pipe6"
pipe7_name = "/tmp/pipe7"

time.sleep(1)

pipe_file_read5 = open(pipe5_name, "r")
pipe_file_read6 = open(pipe6_name, "r")
pipe_file_write = open(pipe7_name, "w")

counter2 = 0
counter1 = 0
ou2_obj = 0
ou1_obj = 0

ou2_flag = 0
ou1_flag = 0

DATA_1 = []
DATA_2 = []

validObj = 0

def processObj(line):
    parts = line.split(":")
    clase = parts[1].replace("', {","").replace("}, '","")
    xmin = float(parts[2].replace("', {","").replace("}, '",""))
    xprom = float(parts[3].replace("', {","").replace("}, '",""))
    xmax = float(parts[4].replace("', {","").replace("}, '",""))
    ymin =float(parts[5].replace("', {","").replace("}, '",""))
    return clase,xmin,xprom,xmax,ymin


while(1):
    if(ou1_flag == 0):
        for line in pipe_file_read6:
            recieve = line
            if(counter1 == 0): #Significa que es el dato donde indican los objetos
                ou1_obj = int(recieve[0])
                #print("Para out.ppm se esperan " + str(ou1_obj) + " objetos")
                if(ou1_obj != 0):
                    counter1 = counter1 + 1
                break
            [class1, xmin1, xprom1, xmax1, ymin1] = processObj(recieve)
            DATA_1.append([class1, xmin1, xprom1, xmax1, ymin1])
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
                #print("Para out2.ppm se esperan " + str(ou2_obj) + " objetos")
                if(ou2_obj != 0):
                    counter2 = counter2 + 1
                break
            [class2, xmin2, xprom2,xmax2, ymin2] = processObj(recieve2)
            DATA_2.append([class2, xmin2, xprom2, xmax2, ymin2])
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
        if(ou1_obj == ou2_obj):
            print("COMRPOBADO, EMPEZANDO TRANSFERENCIA///////")
            DATA_F = ""
            for i in range(len(DATA_1)):
                xclass1 = DATA_1[i][0]
                xclass2 = DATA_2[i][0]
                xprom1 = DATA_1[i][2]
                xprom2 = DATA_2[i][2]
                ymin1 = DATA_1[i][4]
                ymin2 = DATA_2[i][4]
                xmin1 = DATA_1[i][1]
                xmin2 = DATA_2[i][1]
                xmax1 = DATA_1[i][3]
                xmax2 = DATA_2[i][3]

                result_f = (xprom1 + xprom2) / 2
                ymin_f = (ymin1+ymin2)/2
                xmin_f = (xmin1+xmin2)/2
                xmax_f = (xmax1+xmax2)/2

                if (xclass1 == xclass2):

                    if(DATA_F == ""):
                        DATA_F = (xclass1[6])
                    else:
                        DATA_F = DATA_F + (xclass1[6])

                    if (xmin_f < 80):
                         DATA_F = DATA_F + "I" #XMIN A LA IZQUIERDA
                    elif(xmin_f > 112):
                        DATA_F = DATA_F + "D" #XMIN A LA DERECHA
                    else:
                        DATA_F = DATA_F + "C" #XMIN CENTRADO

                    if (result_f < 80):
                        DATA_F = DATA_F + "I" #I de izquierda
                    elif (result_f > 112):
                        DATA_F = DATA_F + "D" #D de derecha
                    else:
                        DATA_F = DATA_F + "C" #C de centro

                    if (xmax_f < 80):
                         DATA_F = DATA_F + "I" #XMIN A LA IZQUIERDA
                    elif(xmax_f > 112):
                        DATA_F = DATA_F + "D" #XMIN A LA DERECHA
                    else:
                        DATA_F = DATA_F + "C" #XMIN CENTRADO

                    if (ymin_f >= 96):
                        DATA_F = DATA_F + "L" #L de lejos
                    else:
                        DATA_F = DATA_F + "K" #K de cerca
                        
                    validObj = validObj + 1

            #Primero mandamos cuantos objetos van
            print("Mandando: " + str(validObj) + DATA_F)
            pipe_file_write.write(str(validObj))
            if(validObj != 0):
                #Mandamos luego los valores
                pipe_file_write.write(DATA_F)
            DATA_F = ""
            pipe_file_write.write("\n")
            pipe_file_write.flush()
            validObj = 0
            
        else:
            print("No se pueden comparar los objetos")
        DATA_1 = []
        DATA_2 = []
        ou2_flag = 0 
        ou1_flag = 0



pipe_file_read5.close()
pipe_file_read6.close()
pipe_file_write.close()
