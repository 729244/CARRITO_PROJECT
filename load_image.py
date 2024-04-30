import torch
from PIL import Image
from datetime import datetime

# Cargar imagen
img_path = "/home/root/img192v3/yolov5/train/images/image13_png.rf.7298cf76bfa678154edb426a2134065b.jpg"
#img_path = "/home/root/image5.png"
img = Image.open(img_path)

#img_crop = img.crop((54,207,261,0))

# Cargar modelo entrenado
modelo = torch.hub.load('/home/root/img192v3/yolov5/','custom', path='/home/root/img192v3/yolov5/runs/train/exp/weights/best-fp16.tflite',source='local')
print("Modelo cargado")
# Realizar predicción
print(type(modelo))
print(type(img))
print(f"Empezando result {datetime.now()}")
results = modelo(img, size=192)
print(f"Fin result {datetime.now()}")
####################
#print("///////////////////////////////////////////////////////////////////////////////////////")
matriz = results.pandas().xyxy[0]
print(matriz)
print(type(matriz))

img_path = "/home/root/img192v3/yolov5/train/images/image88_png.rf.b6d316e921e059c31afa59c6c730edc1.jpg"
img = Image.open(img_path)

print(f"Empezando result {datetime.now()}")
result0 = modelo(img, size=192)
print(f"Fin result {datetime.now()}")

matriz = result0.pandas().xyxy[0]
print(matriz)
print(type(matriz))

img_path = "/home/root/img192v3/yolov5/train/images/image51_png.rf.6d7be4104945d7a0089b1e416a839fb3.jpg"
img = Image.open(img_path)

print(f"Empezando result {datetime.now()}")
result1 = modelo(img, size=192)
print(f"Fin result {datetime.now()}")

matriz = result1.pandas().xyxy[0]
print(matriz)
print(type(matriz))

img_path = "/home/root/img192v3/yolov5/train/images/image100_png.rf.ccbfe400f88ce475c588366ee4c42231.jpg"
img = Image.open(img_path)

print(f"Empezando result {datetime.now()}")
result2 = modelo(img, size=192)
print(f"Fin result {datetime.now()}")

matriz = result2.pandas().xyxy[0]
print(matriz)
print(type(matriz))