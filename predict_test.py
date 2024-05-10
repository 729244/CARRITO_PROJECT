import torch
from PIL import Image
from datetime import datetime
import os

# PIPES FOR DISTANCE
pipe5_name = "/tmp/pipe5"
pipe6_name = "/tmp/pipe6"
#os.mkfifo(pipe5_name)
#os.mkfifo(pipe6_name)

# Cargar modelo entrenado
modelo = torch.hub.load('/home/root/img192v3/yolov5/','custom', path='/home/root/img192v3/yolov5/runs/train/exp/weights/best-fp16.tflite',source='local')
print("Modelo cargado")



p = os.fork()

if(p == 0):
    #3 y 4
    img_path = "/home/root/out2.ppm"
    #img_path = "/home/root/img192v3/yolov5/test/images/image66_png.rf.ce01de0e58b18e8eaeb03aa57afd7ef1.jpg"
    pipe_file_distance = open(pipe6_name,"w")
else:
    #1 y 2
    img_path = "/home/root/out.ppm"
    #img_path = "/home/root/img192v3/yolov5/test/images/image66_png.rf.ce01de0e58b18e8eaeb03aa57afd7ef1.jpg"
    pipe_file_distance = open(pipe5_name,"w")

while(1):
    #red
    img = Image.open(img_path)

    img_crop = img.crop((54,0,261,207))
        
    print(f"\n Empezando result0 {datetime.now()}\n")
    result0 = modelo(img_crop, size=192)
    matriz0 = result0.pandas().xyxy[0]
    print(matriz0)
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
    for i in range(len(para_distancia)):
        pipe_file_distance.write(str(para_distancia[i]))
        pipe_file_distance.write("\n")
        pipe_file_distance.flush()


pipe_file_distance.close()


# Cerrar el archivo                                                                            
                                                              
