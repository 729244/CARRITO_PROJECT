import torch
from PIL import Image
from datetime import datetime
import os

# Cargar modelo entrenado
modelo = torch.hub.load('/home/root/img192v3/yolov5/','custom', path='/home/root/img192v3/yolov5/runs/train/exp/weights/best-fp16.tflite',source='local')
print("Modelo cargado")

# PIPES FOR PIXY
pipe1_name = "/tmp/pipe1"
pipe2_name = "/tmp/pipe2"
pipe3_name = "/tmp/pipe3"
pipe4_name = "/tmp/pipe4"
# PIPES FOR DISTANCE
pipe5_name = "/tmp/pipe5"
pipe6_name = "/tmp/pipe6"

p = os.fork()

if(p == 0):
    #3 y 4
    pipe_file_read = open(pipe3_name, "r")
    pipe_file_write = open(pipe4_name, "w")
    img_path = "/home/root/out2.ppm"
    pipe_file_distance = open(pipe5_name,"w")
else:
    #1 y 2
    pipe_file_write = open(pipe2_name, "w")
    img_path = "/home/root/out.ppm"
    pipe_file_distance = open(pipe6_name,"w")
    pipe_file_read = open(pipe1_name, "r")

while(1):
    for line in pipe_file_read:
        recieve = line
        if(recieve == "1\n"):
            #red
            
            img = Image.open(img_path)

            img_crop = img.crop((54,0,261,207))
                
            #print(f"\n Empezando result0 {datetime.now()}\n")
            result0 = modelo(img_crop, size=192)
            matriz0 = result0.pandas().xyxy[0]
            #print(matriz0)
            #print(type(matriz0))
            #print(f"\nFin result0 {datetime.now()}\n")
            xmin_result0 = result0.pandas().xyxy[0]['xmin']
            xmax_result0 = result0.pandas().xyxy[0]['xmax']
            ymin_result0 = result0.pandas().xyxy[0]['ymin']
            name_result0 = result0.pandas().xyxy[0]['name']
            xmin_array0 = list(xmin_result0)
            xmax_array0 = list(xmax_result0)
            ymin_array0 = list(ymin_result0)
            name_array0 = list(name_result0)


            para_distancia = []
            for xmin, xmax, name, ymin in zip(xmin_array0, xmax_array0, name_array0, ymin_array0):
                xprom = (xmin + xmax) / 2
                CURRENT_TENSOR = ("R:",{name},":",{xprom},":",{ymin},":")
                para_distancia.append(CURRENT_TENSOR)
            ##
            #mandar pipe con string


            pipe_file_distance.write(str(len(para_distancia)) + " Objetos para ")
            if(p == 0):
                pipe_file_distance.write("out2.ppm\n")
            else:
                pipe_file_distance.write("out.ppm\n")
            pipe_file_distance.flush()
            for i in range(len(para_distancia)):
                pipe_file_distance.write(str(para_distancia[i]))
                pipe_file_distance.write("\n")
                pipe_file_distance.flush()
            ##
        
            pipe_file_write.write("1\n")
            pipe_file_write.flush()
            recieve = 0

pipe_file_read.close()
pipe_file_write.close()
pipe_file_distance.close()


# Cerrar el archivo                                                                            
                                                              
