import os

# Crear pipe con nombre
pipe_name = "pipe_file.txt"
os.mkfifo(pipe_name)
#Escribir al pipe con nombre
pipe_file = open(pipe_name, "w")
pipe_file.write("Hola, mundo!")
pipe_file.close()

# Leer del pipe con nombre
pipe_file = open(pipe_name, "r")
data = pipe_file.read()
print(data)
pipe_file.close()


# Crear el archivo de lectura y escritura
#pipe_file = open("pipe_file.txt", "w+")

# Escribir un mensaje en el archivo
#message = "Hola, mundo!"
#pipe_file.write(message)
#pipe_file.flush()

# Mover el puntero al inicio del archivo
#pipe_file.seek(0)

# Leer del archivo
#data = pipe_file.read()
#print(data)

# Cerrar el archivo
#pipe_file.close()



