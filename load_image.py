import torch
from PIL import Image

# Cargar imagen
img_path = "/home/root/image5.png"
img = Image.open(img_path)

# Cargar modelo entrenado
modelo = torch.hub.load('/home/root/yolov5/','custom', path='home/root/yolov5/runs/train/exp/weights/best.pt',source='local')

# Realizar predicción
results = modelo(img)

print(results)