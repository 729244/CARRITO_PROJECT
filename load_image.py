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
else:
    #1 y 2
    pipe_file_read = open(pipe1_name, "r")
    pipe_file_write = open(pipe2_name, "w")

while(1):
    recieve = pipe_file_read.read()
    #hacer proceso de la red

    # Cargar imagen
    img_path = "/home/root/img192v3/yolov5/train/images/image13_png.rf.7298cf76bfa678154edb426a2134065b.jpg"
    #img_path = "/home/root/image5.png"
    img = Image.open(img_path)

    #img_crop = img.crop((54,207,261,0))
        
    print(f"\n Empezando result0 {datetime.now()}\n")
    result0 = modelo(img, size=192)
    matriz0 = result0.pandas().xyxy[0]
    print(matriz0)
    print(type(matriz0))
    print(f"\nFin result0 {datetime.now()}\n")

    img_path = "/home/root/img192v3/yolov5/train/images/image88_png.rf.b6d316e921e059c31afa59c6c730edc1.jpg"
    img = Image.open(img_path)

    print(f"\nEmpezando result1 {datetime.now()}\n")
    result1 = modelo(img, size=192)
    matriz1 = result1.pandas().xyxy[0]
    print(matriz1)
    print(type(matriz1))
    print(f"\nFin result1 {datetime.now()}\n")

    xmin_result0 = result0.pandas().xyxy[0]['xmin']
    xmax_result0 = result0.pandas().xyxy[0]['xmax']
    name_result0 = result0.pandas().xyxy[0]['name']

    xmin_result1 = result1.pandas().xyxy[0]['xmin']
    xmax_result1 = result1.pandas().xyxy[0]['xmax']
    name_result1 = result1.pandas().xyxy[0]['name']

    xmin_array0 = list(xmin_result0)
    xmax_array0 = list(xmax_result0)
    name_array0 = list(name_result0)

    xmin_array1 = list(xmin_result1)
    xmax_array1 = list(xmax_result1)
    name_array1 = list(name_result1)
    
    for xmin, xmax, name in zip(xmin_array0, xmax_array0, name_array0):
        xprom = (xmin + xmax) / 2
        print(f"R0:{name}:{xprom}")
    for xmin, xmax, name in zip(xmin_array1, xmax_array1, name_array1):
        xprom = (xmin + xmax) / 2
        print(f"R1:{name}:{xprom}")
        
    #fin proceso red
    pipe_file_write.write("1")

pipe_file_read.close()
pipe_file_write.close()


# Cerrar el archivo                                                                            
                                                              
