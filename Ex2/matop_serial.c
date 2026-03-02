#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void product(int* A, int* B, int* C, int r, int c){
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            for(int k=0;k<r;k++){
                C[i*c+j] = A[i*c+k]*B[k*c+j];
            }
        }
    }
}

void sum(int* A, int* B, int* C, int r, int c){
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            C[i*c+j]= A[i*c+j] + B[i*c+j];
        }
    }
} 

void diff(int* A, int* B, int* C, int r, int c){
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            C[i*c+j]= A[i*c+j] - B[i*c+j];
        }
    }
} 

void transpose(int* A, int* res, int r, int c){
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            res[i*c+j] = A[j*c+i];
        }
    }
}

int main(){
    int i,r,c,seed;
    printf("Enter the number of rows in mat: ");
    scanf("%d", &r);
    printf("Enter the number of cols in mat: ");
    scanf("%d", &c);
    printf("Enter the seed: ");
    scanf("%d", &seed);
    srand(seed);
    int* A = (int*)malloc(sizeof(int)*r*c);
    int* B = (int*)malloc(sizeof(int)*r*c);
    int* C = (int*)malloc(sizeof(int)*r*c);
    for(i=0;i<r*c;i++){
        A[i] = rand()%100;
    }
    for(i=0;i<r*c;i++){
        B[i] = rand()%100;
    }
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    product(A,B,C,r,c);
    sum(A,B,C,r,c);
    diff(A,B,C,r,c);
    transpose(A,C,r,c);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken total: %f secs\n", elapsed);
}

