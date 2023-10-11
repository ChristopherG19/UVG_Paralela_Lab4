/*
    Universidad del Valle de Guatemala
    Computacion Paralela
    Christopher Garcia 20541
    Laboratorio#4

    - Compilar: gcc -o rie_omp riemann_omp2.c -fopenmp -lm
    - Ejecutar: ./rie_omp a b num_threads op
        - a y b son los límites del intervalo que deseas integrar.
        - num_threads es el número de hilos con los que se trabajaran
        - op es un número que indica la función a utilizar (1 para cuadrática, 2 para cúbica, 3 para seno)
*/

// Se importan librerias
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <sys/time.h>

// Declaración de funciones
double cuadratic_func(double x);
double cubic_func(double x);
double sin_func(double x);
double get_h(double a, double b);
double trapezoides(double (*func)(double), double a, double b, double h, int threads);

// Se define n
int n = 10e6;

int main(int argc, char *argv[])
{
    // Inicialización de variables
    double a = 1.0;
    double b = 1.0;
    int op = 0;
    int num_threads = 10;

    // Verificación de argumentos de línea de comandos
    if (argc > 1){
        a = strtol(argv[1], NULL, 10);
        b = strtol(argv[2], NULL, 10);
        num_threads = strtol(argv[3], NULL, 10);
        op = strtol(argv[4], NULL, 10);

        if(n % num_threads != 0){
            fprintf(stderr, "Número de trapezoides no es múltiplo del número de threads\n");
        }
    }

    // Cálculo del tamaño de paso h
    double h = get_h(a, b);
    double resultado = 0.0;

    // Inicio del contador de tiempo
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Selección de la función a integrar
    if(op == 1){
        resultado = trapezoides(cuadratic_func, a, b, h, num_threads);
    } else if(op == 2){
        resultado = trapezoides(cubic_func, a, b, h, num_threads);
    } else if(op == 3){
        resultado = trapezoides(sin_func, a, b, h, num_threads);
    } else {
        printf("Error: Opción no válida.\n");
    }

    // Fin del contador de tiempo
    gettimeofday(&end, NULL);
    double execution_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000;

    // Impresión del resultado y tiempo de ejecución
    printf("Con n = %d, nuestra aproximación de la integral de %f a %f es = %f\n", n, a, b, resultado);
    printf("Tiempo de ejecución: %f segundos\n", execution_time);

    return 0;
}

// Función para calcular la integral de una función en el intervalo [a, b]
double trapezoides(double (*func)(double), double a, double b, double h, int threads) {
    double sum = 0.0;
    double sumF = 0.0;
    int n_local = n / threads;
    
    #pragma omp parallel num_threads(threads)
    {
        int ID_thread = omp_get_thread_num();
        double a_local = a + ID_thread * n_local * h;
        double b_local = a_local + n_local * h;
        sumF = (func(a_local) + func(b_local)) / 2.0;

        for(int i = 1; i < n_local; i++){
            double x = a_local + i * h;
            sumF += func(x);
        }

        sumF *= h;

        #pragma omp critical
        sum += sumF;
    }
    return sum;
}

// Función para calcular el tamaño de paso h
double get_h(double a, double b){
    return (b-a)/n;
}

// Función cuadrática (x^2)
double cuadratic_func(double x){
    return pow(x,2);
}

// Función cúbica (2x^3)
double cubic_func(double x){
    return 2.0 * pow(x,3);
}

// Función seno (sin(x))
double sin_func(double x){
    return sin(x);
}
