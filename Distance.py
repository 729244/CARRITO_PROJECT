
from ctypes import sizeof


pipe1_name = "/tmp/pipe1"
pipe2_name = "/tmp/pipe2"

pipe_file_read_1 = open(pipe1_name, "r")
pipe_file_read_2 = open(pipe2_name, "r")

tensor_1 = []
tensor_2 = []

tensor_1 = pipe_file_read_1.read().split("R")
tensor_2 = pipe_file_read_2.read().split("R")

Classes_1 = []
Classes_2 = []

Classes_1 = [tensor_1[i].split(":")[1]]
Classes_2 = [tensor_2[i].split(":")[1]]


