#!/bin/bash 

mkfifo /tmp/pipe5
mkfifo /tmp/pipe6
python3 predict_test.py &
python3 DistanceSPI.py &
