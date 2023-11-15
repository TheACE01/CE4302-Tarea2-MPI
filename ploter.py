import os
import numpy as np
import matplotlib.pyplot as plt


# Define the text file path
file_path = "mpi_data.txt"

# Iterate the text file to get the amount of data recorded
MAX_DATA = 0
with open(file_path, 'r') as file:
    lines = file.readlines()
    for line in lines:
        if line.strip(): # Ignore empty lines
            MAX_DATA += 1

# Create empty vectors
time_vector = np.zeros(MAX_DATA)
message_size_vector = np.zeros(MAX_DATA)

# Extract the data from the text file
with open(file_path, 'r') as file:
    counter = 0
    lines = file.readlines()
    for line in lines:
        if line.strip(): # Ignore empty lines
            time, message_size = map(float, line.strip().split(','))
            time_vector[counter] = time
            message_size_vector[counter] = message_size
            counter += 1

# Print the current data
print(time_vector)
print(message_size_vector)

# Plot the Time VS MessageSize
plt.figure(figsize=(8, 6), num="Parte C")
plt.plot(message_size_vector, time_vector, marker='o', linestyle='-', color='b')
plt.title('Tiempo de ejecución vs Tamaño del mensaje')
plt.xlabel('Tamaño del mensaje [Cantidad de elementos]')
plt.ylabel('Tiempo de ejecución [S]')
plt.grid(True)
#plt.show()
plt.savefig('output.png')


