#!/bin/bash 

mkfifo /tmp/pipe5
mkfifo /tmp/pipe6
./get_raw_frame &
python3 load_image.py &
python3 DistanceSPI.py &
