#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define SIZE 4

int A[SIZE][SIZE], B[SIZE][SIZE], PRO[SIZE][SIZE], SUM[SIZE][SIZE];

void* matmul(void* argv){
	for(int i=0;i<SIZE;i++){
		for(int j=0;j<SIZE;j++){
			for(int k=0;k<SIZE;k++){
				PRO[i][j] = A[i][k]*B[j][k];
			}
		}
	}
	return NULL;
}

void matsum(void* argv){
	for(int i=0;i<SIZE;i++){
		for(int j=0;j<SIZE;j++){
			SUM[i][j] = A[i][j]+B[i][j];
		}
	}
	return NULL;
}
int main(){
	printf("Enter the values of matrix 1:\n");
	for(int i=0;i<SIZE;i++){
		for(int j=0;j<SIZE;j++){
			printf("A[%d][%d]: ",i,j);
			scanf("%d", &A[i][j]);
		}
	}
	printf("Enter the values of matrix 2:\n");
	for(int i=0;i<SIZE;i++){
		for(int j=0;j<SIZE;j++){
			printf("B[%d][%d]: ", i,j);
			scanf("%d", &B[i][j]);
		}
	}
	pthread_t add_thread, mul_thread;
	pthread_create(&add_thread, NULL, matsum, NULL);
	pthread_create(&mul_thread, NULL, matmul, NULL);
	pthread_join(add_thread, NULL);
	pthread_join(mul_thread, NULL);

	printf("Matrix addition result:\n");
	for(int i=0;i<SIZE:i++){
		for(int j=0;j<SIZE;j++){
			printf("%d ", SUM[i][j]);
		}
		printf("\n");
	}

	printf("Matrix multiplication result:\n");
	for(int i=0;i<SIZE:i++){
		for(int j=0;j<SIZE:j++){
			printf("%d ", PRO[i][j]);
		}
		printf("\n");
	}

}
