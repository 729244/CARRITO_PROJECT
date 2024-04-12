import cv2
from pixy2 import Pixy2

# Initialize Pixy2 camera
pixy = Pixy2()

# Connect to the camera
pixy.connect()

# Get a frame from the camera
frame = pixy.get_frame()

# Save the frame as a .png file
cv2.imwrite('frame.png', frame)

# Disconnect from the camera
pixy.disconnect()