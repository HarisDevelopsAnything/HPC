#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

void multiply_row(int row, int *A, int *B, int *C, int c1, int c2) {
    for (int j = 0; j < c2; j++) {
        int sum = 0;
        for (int k = 0; k < c1; k++) {
            sum += A[row * c1 + k] * B[k * c2 + j];
        }
        C[row * c2 + j] = sum;
    }
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
    int shmid_A, shmid_B, shmid_C;
    int *A, *B, *C;
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
    
    shmid_A = shmget(1104, r1*c1*sizeof(int), IPC_CREAT | 0666);
    shmid_B = shmget(1105, r2*c2*sizeof(int), IPC_CREAT | 0666);
    shmid_C = shmget(1106, r1*c2*sizeof(int), IPC_CREAT | 0666);
    A = (int *)shmat(shmid_A, NULL, 0);
    B = (int *)shmat(shmid_B, NULL, 0);
    C = (int *)shmat(shmid_C, NULL, 0);
    int count = 0;
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
    clock_t start_normal = clock();
    mat_multiply(A, B, C, r1, c1, c2);
    clock_t end_normal = clock();
    double time_normal = ((double)(end_normal - start_normal)) / CLOCKS_PER_SEC;
    
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
    
    printf("\nParallel Multiplication\n");
    clock_t start_parallel = clock();
    printf("Multiplying matrices using child processes...\n");
    for (int i = 0; i < r1; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            multiply_row(i, A, B, C, c1, c2);
            shmdt(A);
            shmdt(B);
            shmdt(C);
            exit(0); 
        }
    }    for (int i = 0; i < r1; i++) {
        wait(NULL);
    }
    clock_t end_parallel = clock();
    double time_parallel = ((double)(end_parallel - start_parallel)) / CLOCKS_PER_SEC;
    
    printf("\nResult:\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            printf("%d ", C[i * c2 + j]);
        }
        printf("\n");
    }
    printf("Time taken: %f seconds\n", time_parallel);

    shmdt(A);
    shmdt(B);
    shmdt(C);
    shmctl(shmid_A, IPC_RMID, NULL);
    shmctl(shmid_B, IPC_RMID, NULL);
    shmctl(shmid_C, IPC_RMID, NULL);

    return 0;
}
