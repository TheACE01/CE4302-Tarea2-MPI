#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int pid, np;
    int elements_per_process;
    int vector_size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <vector_size>\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    vector_size = atoi(argv[1]);
    elements_per_process = vector_size / np;

    int *a = (int *)malloc(vector_size * sizeof(int));
    int *b = (int *)malloc(vector_size * sizeof(int));
    int *c = (int *)malloc(vector_size * sizeof(int));

    // Inicializar los vectores a y b (puedes cambiar esto según tus necesidades)
    for (int i = 0; i < vector_size; i++) {
        a[i] = vector_size - 1;
        b[i] = vector_size + 1;
    }

     // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    printf("Process %d from processor %s, rank %d out of %d processors\n",
           pid, processor_name, pid, np);

    // Distribuir partes de los vectores a los otros procesos 
    MPI_Scatter(a, elements_per_process, MPI_INT, a, elements_per_process, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(b, elements_per_process, MPI_INT, b, elements_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time, end_time;

    // Medir el tiempo de inicio
    start_time = MPI_Wtime();

    // Sumar las partes de los vectores
    for (int i = 0; i < elements_per_process; i++) {
        c[i] = a[i] + b[i];
    }

    // Recolectar los resultados parciales en el proceso raíz
    MPI_Gather(c, elements_per_process, MPI_INT, c, elements_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    // Medir el tiempo de finalización
    end_time = MPI_Wtime();

    // El proceso raíz imprime el resultado
    if (pid == 0) {
        printf("Final result after gathering all partial results:\n");
        // Imprimir los primeros y últimos valores del resultado
        printf("Summary of result: First values [");
        for (int i = 0; i < 5; i++) {
            printf("%d ", c[i]);
        }
        printf("], Last values [");
        for (int i = vector_size - 5; i < vector_size; i++) {
            printf("%d ", c[i]);
        }
        printf("]\n");

        // Imprimir el tiempo transcurrido
        printf("Time elapsed: %f seconds\n", end_time - start_time);

        // Guardar el tiempo y el tamaño del mensaje en un archivo
        char filename[50] = "mpi_data.txt";
        FILE *fp = fopen(filename, "a");

        if (fp == NULL) {
            printf("Error al abrir el archivo %s\n", filename);
            return 1;
        }

        // Guardar el tiempo y el tamaño del mensaje en el archivo
        fprintf(fp, "%f,%d\n", end_time - start_time, elements_per_process);

        // Cerrar el archivo
        fclose(fp);
    }

    free(a);
    free(b);
    free(c);

    MPI_Finalize();

    return 0;
}

