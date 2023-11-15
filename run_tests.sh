#!/bin/bash

# Tamaños de vector para realizar pruebas
vector_sizes=(999 5001 7502 10002 25002 50001 750002 100002 250002 500001 750002 1000002)

# Iterar sobre diferentes tamaños de vector
for size in "${vector_sizes[@]}"; do
    echo "Running MPI program with vector size: $size"
    mpirun -n 3 -H localhost,node2,node3 ./mpi_vector_add $size
    echo "--------------------------------------"
done

