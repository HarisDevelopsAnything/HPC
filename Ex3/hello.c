#include <mpi.h>
#include <stdio.h>

int main() {
    int rank, size;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Hello from process %d out of %d processes\n", rank, size);

    MPI_Finalize();
    return 0;
}
