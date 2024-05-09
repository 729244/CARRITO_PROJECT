import torch
from PIL import Image
from datetime import datetime
import os

# Cargar modelo entrenado
modelo = torch.hub.load('/home/root/img192v3/yolov5/','custom', path='/home/root/img192v3/yolov5/runs/train/exp/weights/best-fp16.tflite',source='local')
print("Modelo cargado")

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
    img_path = "/home/root/out2.png"
else:
    #1 y 2
    pipe_file_read = open(pipe1_name, "r")
    pipe_file_write = open(pipe2_name, "w")
    img_path = "/home/root/out.png"

while(1):
    for line in pipe_file_read:
        recieve = line
        if(recieve == "1\n"):
            #red
            img = Image.open(img_path)

            img_crop = img.crop((54,0,261,207))
                
            print(f"\n Empezando result0 {datetime.now()}\n")
            result0 = modelo(img, size=192)
            matriz0 = result0.pandas().xyxy[0]
            print(matriz0)
            print(type(matriz0))
            print(f"\nFin result0 {datetime.now()}\n")
            xmin_result0 = result0.pandas().xyxy[0]['xmin']
            xmax_result0 = result0.pandas().xyxy[0]['xmax']
            name_result0 = result0.pandas().xyxy[0]['name']
            xmin_array0 = list(xmin_result0)
            xmax_array0 = list(xmax_result0)
            name_array0 = list(name_result0)
            for xmin, xmax, name in zip(xmin_array0, xmax_array0, name_array0):
                xprom = (xmin + xmax) / 2
                print(f"R0:{name}:{xprom}")
            ##
            pipe_file_write.write("1\n")
            pipe_file_write.flush()
            recieve = 0

pipe_file_read.close()
pipe_file_write.close()


# Cerrar el archivo                                                                            
                                                              
