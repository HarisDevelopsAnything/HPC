#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    int *A, *B, *C;
    int r1, c1, r2, c2;
    int seed;
    printf("No. of rows in A: ");
    scanf("%d", &r1);
    printf("No. of columns in A: ");
    scanf("%d", &c1);
    printf("No. of rows in B: ");
    scanf("%d", &r2);
    printf("No. of columns in B: ");
    scanf("%d", &c2);
    printf("Enter seed: ");
    scanf("%d", &seed);
    
    if(c1 != r2) {
        printf("Incompatible for multiplication!\n");
        exit(0);
    }
    srand(seed);
    A = (int *)malloc(r1 * c1 * sizeof(int));
    B = (int *)malloc(r2 * c2 * sizeof(int));
    C = (int *)malloc(r1 * c2 * sizeof(int));
    
    for (int i = 0; i < r1*c1; i++) {
        A[i]= rand()%100;
    }
    
    for (int i = 0; i < r2*c2; i++) {
        B[i] = rand()%100;
    }
    
    
    printf("\nSequential Multiplication\n");
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    mat_multiply(A, B, C, r1, c1, c2);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken: %f seconds\n", time_taken);
    
    free(A);
    free(B);
    free(C);
    
    return 0;
}
