#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Definiciones generales
    int pid, np;                // Identificador y número total de procesos
    int elements_per_process;   // Número de elementos por proceso
    int vector_size;            // Tamaño total del vector
    MPI_Status status;          // Estructura para almacenar el estado de la operación MPI

    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid); // Obtener el rango (identificador) del proceso actual
    MPI_Comm_size(MPI_COMM_WORLD, &np);  // Obtener el número total de procesos

    // Manejo de argumentos de línea de comandos
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <vector_size>\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);   // Abortar MPI si no se proporciona el tamaño del vector
    }
    
    // Asignación del tamaño del vector y cálculo de elementos por proceso
    vector_size = atoi(argv[1]);
    elements_per_process = vector_size / np;

    // Asignación de memoria para los vectores
    int *a = (int *)malloc(vector_size * sizeof(int));
    int *b = (int *)malloc(vector_size * sizeof(int));
    int *c = (int *)malloc(vector_size * sizeof(int));

    // Inicializar los vectores a y b
    for (int i = 0; i < vector_size; i++) {
        a[i] = vector_size - 1;
        b[i] = vector_size + 1;
    }

    // Obtener el nombre del procesador
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    printf("Process %d from processor %s, rank %d out of %d processors\n",
           pid, processor_name, pid, np);

    // Distribuir partes de los vectores a los otros procesos 
    /* 
     * MPI Scatter toma los siguientes argumentos en este orden:
     *   - Dirección inicial del buffer de salida
     *   - Número de elementos que se envía a cada proceso
     *   - Tipo de dato que se va a enviar
     *   - Direción del buffer de recepción
     *   - Número de elementos que espera recibir cada proceso
     *   - Tipo de datos de los elementos a recibir 
     *   - Rango (rank) del proceso raíz (el que realizará el envío)
     *   - Comunicador por el que realizar la comunicación.
     */
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
    /*
     * MPI_Gather toma los mismos argumentos que MPI_Scatter, pero para recibir los datos 
     */
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

    // Liberar memoria
    free(a);
    free(b);
    free(c);

    // Finalizar MPI
    MPI_Finalize();

    return 0;
}

