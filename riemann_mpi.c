/*
    Universidad del Valle de Guatemala
    Computacion Paralela
    Christopher Garcia 20541
    Laboratorio#4

    - Compilar: mpicc -o rie_mpi riemann_mpi.c -lm
    - Ejecutar: mpirun -np 4 ./rie_mpi a b
        - a y b son los límites del intervalo que deseas integrar.
*/

// Se importan librerias
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <sys/time.h>

// Declaración de funciones
double cuadratic_func(double x);
double trapezoides(double (*func)(double), double a, double b, int n, double h);

// Número de subdivisiones (n)
int n = 10e6;

int main(int argc, char *argv[])
{
    // Inicialización de MPI
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // límites del intervalo
    double a = 1.0;
    double b = 1.0;

    // Verificación de argumentos de línea de comandos
    if (argc > 1) {
        a = atof(argv[1]);
        b = atof(argv[2]);
    }

    double h = (b - a) / n;
    double local_sum = 0.0;

    // Inicio del contador de tiempo
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Cálculo de la integral en paralelo
    for (int i = rank; i < n; i += size) {
        double x = a + i * h;
        local_sum += trapezoides(cuadratic_func, x, x + h, 1, h);
    }

    double global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Fin del contador de tiempo
    gettimeofday(&end, NULL);
    double execution_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000;

    // Impresión del resultado y tiempo de ejecución
    if (rank == 0) {
        printf("Con n = %d, nuestra aproximación de la integral de %f a %f es = %f\n", n, a, b, global_sum);
        printf("Tiempo de ejecución: %f segundos\n", execution_time);
    }

    // Finalización de MPI
    MPI_Finalize();

    return 0;
}

// Función para calcular la integral de una función en el intervalo [a, b]
double trapezoides(double (*func)(double), double a, double b, int n, double h) {
    double sum = 0.0;
    sum = (func(a) + func(b)) / 2.0;

    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += func(x);
    }

    sum *= h;
    return sum;
}

// Función cuadrática (x^2)
double cuadratic_func(double x) {
    return x * x;
}

