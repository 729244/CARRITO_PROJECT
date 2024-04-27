import torch
from PIL import Image
from datetime import datetime

# Cargar imagen
img_path = "/home/root/img192/yolov5/train/images/image13_png.rf.beac022b3c5d0c8a57be41b763af0b1a.jpg"
#img_path = "/home/root/image5.png"
img = Image.open(img_path)

#img_crop = img.crop((54,207,261,0))

# Cargar modelo entrenado
modelo = torch.hub.load('/home/root/img192/yolov5/','custom', path='/home/root/img192/yolov5/runs/train/exp/weights/best-fp16.tflite',source='local')
print("Modelo cargado")
# Realizar predicción
print(f"Empezando result {datetime.now()}")
results = modelo(img, size=192)
print(f"Fin result {datetime.now()}")
####################
#print("///////////////////////////////////////////////////////////////////////////////////////")
matriz = results.pandas().xyxy[0]
print(matriz)
print(type(matriz))

img_path = "/home/root/img192/yolov5/train/images/image84_png.rf.cd44160bc07745b19fa51d611bf53bb9.jpg"
img = Image.open(img_path)

print(f"Empezando result {datetime.now()}")
result0 = modelo(img, size=192)
print(f"Fin result {datetime.now()}")

img_path = "/home/root/img192/yolov5/train/images/image162_png.rf.d287c58e0aad244eb1dc91686738ccd9.jpg"
img = Image.open(img_path)

print(f"Empezando result {datetime.now()}")
result1 = modelo(img, size=192)
print(f"Fin result {datetime.now()}")

img_path = "/home/root/img192/yolov5/train/images/image184_png.rf.31ddba6e10ce0532dc7c1498f464c49f.jpg"
img = Image.open(img_path)

print(f"Empezando result {datetime.now()}")
result2 = modelo(img, size=192)
print(f"Fin result {datetime.now()}")

