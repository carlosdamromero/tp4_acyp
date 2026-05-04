#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    // Variables locales para cada proceso
    long long int lanzamientos_local, aciertos_local = 0;
    // Variables globales para recolectar resultados (solo útiles en el proceso 0)
    long long int aciertos_global = 0, total_lanzamientos_global = 0;
    double x, y, pi_estimado;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // MODIFICACIÓN: Cada proceso define una carga distinta.
    // Ejemplo: El proceso 0 lanza 100k, el proceso 1 lanza 200k, etc.
    lanzamientos_local = (rank + 1) * 100000;

    // Semilla única combinando tiempo y rango para evitar repeticiones
    std::srand(std::time(nullptr) + rank);

    // Simulación local
    for (long long int i = 0; i < lanzamientos_local; ++i) {
        x = static_cast<double>(std::rand()) / RAND_MAX;
        y = static_cast<double>(std::rand()) / RAND_MAX;
        if (x*x + y*y <= 1.0) {
            ++aciertos_local;
        }
    }

    std::cout << "Proceso " << rank << " finalizó: " << lanzamientos_local 
              << " lanzamientos, " << aciertos_local << " aciertos." << std::endl;

    // REDUCCIÓN 1: Sumar todos los aciertos locales en 'aciertos_global'
    MPI_Reduce(&aciertos_local, &aciertos_global, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // REDUCCIÓN 2: Sumar todos los lanzamientos locales en 'total_lanzamientos_global'
    // Esto es necesario porque ahora no todos lanzaron la misma cantidad.
    MPI_Reduce(&lanzamientos_local, &total_lanzamientos_global, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Cálculo final usando los totales globales
        pi_estimado = 4.0 * static_cast<double>(aciertos_global) / total_lanzamientos_global;

        std::cout << "\n========================================" << std::endl;
        std::cout << "       ESTIMACIÓN DE PI (MONTECARLO)      " << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Procesos totales:      " << size << std::endl;
        std::cout << "Lanzamientos totales:  " << total_lanzamientos_global << std::endl;
        std::cout << "Aciertos totales:      " << aciertos_global << std::endl;
        std::cout << "Valor estimado de PI:  " << pi_estimado << std::endl;
        std::cout << "Error respecto a PI:   " << pi_estimado - 3.141592653589793 << std::endl;
        std::cout << "========================================\n" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
