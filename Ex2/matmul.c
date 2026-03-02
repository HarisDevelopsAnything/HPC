#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

void multiply_row(int row, int *A, int *B, int *C, int c1, int c2, double *times) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int j = 0; j < c2; j++) {
        int sum = 0;
        for (int k = 0; k < c1; k++) {
            sum += A[row * c1 + k] * B[k * c2 + j];
        }
        C[row * c2 + j] = sum;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    times[row] = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

void mat_multiply(int *A, int *B, int *C, int r1, int c1, int c2) {
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            int sum = 0;
            for (int k = 0; k < c1; k++) {
                sum += A[i * c1 + k] * B[k * c2 + j];
            }
            C[i * c2 + j] = sum;
        }
    }
}

int main() {
    int shmid_A, shmid_B, shmid_C, shmid_times;
    int *A, *B, *C;
    double *times;
    int pid;
    int r1,c1,r2,c2;
    printf("No. of rows in A: ");
    scanf("%d", &r1);
    printf("No. of columns in A: ");
    scanf("%d", &c1);
    printf("No. of rows in B: ");
    scanf("%d", &r2);
    printf("No. of columns in B: ");
    scanf("%d", &c2);
    if(c1!=r2){
        printf("Incompatible for multiplication!");
        exit(0);
    }
    A = (int *)malloc(r1 * c1 * sizeof(int));
    B = (int *)malloc(r2 * c2 * sizeof(int));
    C = (int *)malloc(r1 * c2 * sizeof(int));
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            printf("A[%d][%d]: ", i,j);
            scanf("%d", &A[i*c1+j]);
        }
    }
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            C[i * c2 + j] = 0;
        }
    }
    for(int i=0;i<r2;i++){
        for(int j=0;j<c2;j++){
            printf("B[%d][%d]: ", i,j);
            scanf("%d", &B[i*c2+j]);
        }
    }
    printf("Matrix A:\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) printf("%d ", A[i * c1 + j]);
        printf("\n");
    }
    printf("Matrix B:\n");
    for (int i = 0; i < r2; i++) {
        for (int j = 0; j < c2; j++) printf("%d ", B[i * c2 + j]);
        printf("\n");
    }
    // Normal sequential multiplication
    printf("\nSequential Multiplication\n");
    struct timespec start_normal, end_normal;
    clock_gettime(CLOCK_MONOTONIC, &start_normal);
    mat_multiply(A, B, C, r1, c1, c2);
    clock_gettime(CLOCK_MONOTONIC, &end_normal);
    double time_normal = (end_normal.tv_sec - start_normal.tv_sec) + (end_normal.tv_nsec - start_normal.tv_nsec) / 1e9;
    
    printf("Result:\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            printf("%d ", C[i * c2 + j]);
        }
        printf("\n");
    }
    printf("Time taken: %f seconds\n", time_normal);
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            C[i * c2 + j] = 0;
        }
    }
    
    shmid_A = shmget(1104, r1*c1*sizeof(int), IPC_CREAT | 0666);
    shmid_B = shmget(1105, r2*c2*sizeof(int), IPC_CREAT | 0666);
    shmid_C = shmget(1106, r1*c2*sizeof(int), IPC_CREAT | 0666);
    shmid_times = shmget(1107, r1*sizeof(double), IPC_CREAT | 0666);
    int* shared_A = (int *)shmat(shmid_A, NULL, 0);
    int* shared_B = (int *)shmat(shmid_B, NULL, 0);
    int* shared_C = (int *)shmat(shmid_C, NULL, 0);
    double* shared_times = (double *)shmat(shmid_times, NULL, 0);
    int count = 0;

    for (int i = 0; i < r1 * c1; i++) {
        shared_A[i] = A[i];
    }
    for (int i = 0; i < r2 * c2; i++) {
        shared_B[i] = B[i];
    }
    for (int i = 0; i < r1 * c2; i++) {
        shared_C[i] = 0;
    }
    
    printf("\nParallel Multiplication\n");
    printf("Multiplying matrices using child processes...\n");
    struct timespec start_parallel, end_parallel;
    clock_gettime(CLOCK_MONOTONIC, &start_parallel);
    for (int i = 0; i < r1; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            multiply_row(i, shared_A, shared_B, shared_C, c1, c2, shared_times);
            shmdt(shared_A);
            shmdt(shared_B);
            shmdt(shared_C);
            shmdt(shared_times);
            exit(0);
        }
    }
    
    for (int i = 0; i < r1; i++) {
        wait(NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end_parallel);
    double time_parallel = (end_parallel.tv_sec - start_parallel.tv_sec) + (end_parallel.tv_nsec - start_parallel.tv_nsec) / 1e9;
    
    // Also calculate max individual process time for comparison
    double max_process_time = 0.0;
    for (int i = 0; i < r1; i++) {
        if (shared_times[i] > max_process_time) {
            max_process_time = shared_times[i];
        }
    }

    printf("\nResult:\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            printf("%d ", shared_C[i * c2 + j]);
        }
        printf("\n");
    }
    printf("Time taken (total parallel): %f seconds\n", time_parallel);
    printf("Time taken (max single process): %f seconds\n", max_process_time);

    shmdt(shared_A);
    shmdt(shared_B);
    shmdt(shared_C);
    shmdt(shared_times);
    shmctl(shmid_A, IPC_RMID, NULL);
    shmctl(shmid_B, IPC_RMID, NULL);
    shmctl(shmid_C, IPC_RMID, NULL);
    shmctl(shmid_times, IPC_RMID, NULL);

    return 0;
}
