/*
    Universidad del Valle de Guatemala
    Computacion Paralela
    Christopher Garcia 20541
    Laboratorio#4

    - Compilar: gcc -o rie riemann.c -fopenmp -lm
    - Ejecutar: ./rie a b op
        - a y b son los límites del intervalo que deseas integrar.
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
double trapezoides(double (*func)(double), double a, double b, double h);

// Se define n
int n = 10e6;

int main(int argc, char *argv[])
{
    // Inicialización de variables
    double a = 1.0;
    double b = 1.0;
    int op = 0;

    // Verificación de argumentos de línea de comandos
    if (argc > 1){
        a = strtol(argv[1], NULL, 10);
        b = strtol(argv[2], NULL, 10);
        op = strtol(argv[3], NULL, 10);
    }

    // Cálculo del tamaño de paso h
    double h = get_h(a, b);
    double resultado = 0.0;

    // Inicio del contador de tiempo
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Selección de la función a integrar
    if(op == 1){
        resultado = trapezoides(cuadratic_func, a, b, h);
    } else if(op == 2){
        resultado = trapezoides(cubic_func, a, b, h);
    } else if(op == 3){
        resultado = trapezoides(sin_func, a, b, h);
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
double trapezoides(double (*func)(double), double a, double b, double h) {
    double sum = 0.0;
    sum = (func(a) + func(b)) / 2.0;

    for(int i = 1; i < n; i++){
        double x = a + i * h;
        sum += func(x);
    }

    sum *= h;
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
