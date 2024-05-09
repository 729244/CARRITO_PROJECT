#!/bin/bash 

./get_raw_frame &
python3 load_image.py &
python3 DistanceSPI.py &
