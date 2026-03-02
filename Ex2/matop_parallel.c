#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

void product(int* A, int* B, int* C, int r, int c, double* times){
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            for(int k=0;k<r;k++){
                C[i*c+j] = A[i*c+k]*B[k*c+j];
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double el = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1e9;
    times[0] = el;
}

void sum(int* A, int* B, int* C, int r, int c, double* times){
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            C[i*c+j]= A[i*c+j] + B[i*c+j];
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double el = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1e9;
    times[1] = el;
} 

void diff(int* A, int* B, int* C, int r, int c, double* times){
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            C[i*c+j]= A[i*c+j] - B[i*c+j];
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double el = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1e9;
    times[2] =  el;
} 

void transpose(int* A, int* res, int r, int c, double* times){
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            res[i*c+j] = A[j*c+i];
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double el = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1e9;
    times[3] = el;
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
    int shmid_times = shmget(1104, 5*sizeof(double), IPC_CREAT | IPC_EXCL | 0666);
    double* times = (double*)shmat(shmid_times, NULL, 0);
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
    int pid;
    pid = fork();
    if(pid < 0){
        printf("Fork error for mult\n");
    }
    else if(pid==0){
        product(A,B,C,r,c,times);
        printf("Product done.\n");
        exit(0);
    }
    pid = fork();
    if(pid < 0){
        printf("Fork error for add\n");
    }
    else if(pid==0){
        sum(A,B,C,r,c,times);
        printf("Sum done.\n");
        exit(0);
    }
    pid = fork();
    if(pid < 0){
        printf("Fork error for sub\n");
    }
    else if(pid==0){
        diff(A,B,C,r,c,times);
        printf("Diff done.\n");
        exit(0);
    }
    pid = fork();
    if(pid < 0){
        printf("Fork error for transpose\n");
    }
    else if(pid==0){
        transpose(A,C,r,c,times);
        printf("Transpose done.\n");
        exit(0);
    }
    for(i=0;i<4;i++){
        wait(NULL);
    }
    double max = 0;
    for(i=0;i<4;i++){
        if(times[i] > max) max=times[i];
    }
    shmdt(times);
    shmctl(shmid_times, IPC_RMID, NULL);
    printf("Time taken total: %f secs\n", max);
}

