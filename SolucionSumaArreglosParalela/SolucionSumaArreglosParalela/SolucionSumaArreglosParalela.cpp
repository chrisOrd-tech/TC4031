// SolucionSumaArreglosParalela.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#include <iostream>
#include <iomanip> // Biblioteca para manipuladores de formato
#include <random> // Librería para generar números aleatorios
#include <chrono>
#ifdef _OPENMP // Comprueba si OpenMP ha sido agregado al proyecto
#include <omp.h> // Librería OpenMP
#else
#define omp_get_thread_num() 0
#endif

#define N 50000 // Cantidad de los arreglos
#define chunk 100 // Porción de los arreglos que cada hilo procesará
#define mostrar 10 // Cantidad de datos a imprimir en consola

void imprimeArreglo(float* d);

int main()
{
    std::cout << "Sumando arreglos en paralelo!\n";

    // Declaración de números aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib_a(0.0, 100.0); // Rango para el arreglo a (0.0 a 100.0)
    std::uniform_real_distribution<> distrib_b(50.0, 150.0); // Rango para el arreglo b (50.0 a 150.0)

    float a[N], b[N], c[N]; // Declaración de los arreglos a utilizar
    int i; // Índice para usar en los ciclos for

    // Llenado de los arreglos 'a' y 'b' con números aleatorios
    for (i = 0; i < N; i++) {
        a[i] = distrib_a(gen);
        b[i] = distrib_b(gen);
    }

    int pedazos = chunk;

    // Medir tiempo de la operación secuencial
    auto start_seq = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        c[i] = a[i] + b[i]; // Suma secuencial
    }
    auto end_seq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seq = end_seq - start_seq;


    // Medir tiempo de la operación en paralelo
    auto start_par = std::chrono::high_resolution_clock::now();

    // Ciclo for para realizar la suma de arreglos 'a' y 'b', cuyo resultado será almacenado en 'c'
    // Las variables 'a', 'b', 'c' y 'pedazos' son compartidas entre los hilos
    // La variable 'i' es privada para cada hilo, evitando conflictos entre hilos
    // El trabajo se divide en bloques de tamaño 'pedazos', asignando cada bloque de manera estática a los hilos disponibles
#ifdef _OPENMP
    omp_set_num_threads(8);
#endif // DEBUG


#pragma omp parallel for \
    shared(a, b, c, pedazos) private(i) \
    schedule(static, pedazos)

    for (i = 0;i < N;i++) {
        c[i] = a[i] + b[i];
    }
    auto end_par = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_par = end_par - start_par;


    std::cout << "Imprimiendo los primeros " << mostrar << " valores del arreglo 'a': " << std::endl;
    imprimeArreglo(a);
    std::cout << "Imprimiendo los primeros " << mostrar << " valores del arreglo 'b': " << std::endl;
    imprimeArreglo(b);
    std::cout << "Imprimiendo los primeros " << mostrar << " valores del arreglo 'c': " << std::endl;
    imprimeArreglo(c);

    std::cout << "\n\n***********************************************************" << std::endl;
    std::cout << "Parametros\n" << "N: " << N << "\nChunk: " << chunk << "\n" << std::endl;
    std::cout << std::scientific << std::setprecision(10); // Formato científico con 7 decimales
    std::cout << "Tiempo en secuencial:\t" << elapsed_seq.count() << " segundos" << std::endl;
    std::cout << "Tiempo en paralelo:\t" << elapsed_par.count() << " segundos" << std::endl;

    std::chrono::duration<double> time_diff = elapsed_seq - elapsed_par;
    std::cout << "\nDiferencia entre serie y paralelo:\t" << time_diff.count() << " segundos" << std::endl;
}

void imprimeArreglo(float* d) {
    for (int x = 0; x < mostrar; x++) {
        std::cout << d[x] << " - ";
    }
    std::cout << std::endl;
}