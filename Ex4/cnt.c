#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>

int main(){
	int comm_sz;
	int my_rank;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int n= 100;
	int sub_len = n/comm_sz;
	int* arr = NULL;
	int* sub_arr = (int*)malloc(sizeof(int)*sub_len);
	int local_freq = 0;
	int total_freq = 0;
	int key;
	int i;
	if(my_rank==0){
		arr = (int*)malloc(sizeof(int)*n);
		for(i=0;i<n;i++){
			arr[i] = rand()%10+1;
			printf("%d ", arr[i]);
		}
		printf("\n");
		printf("Freq to find:\n ");
		scanf("%d", &key);
	}
	MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(arr, sub_len, MPI_INT, sub_arr, sub_len, MPI_INT, 0, MPI_COMM_WORLD);

	for(i=0;i<sub_len;i++){
		if(sub_arr[i] == key){
			local_freq++;
		}
	}

	MPI_Reduce(&local_freq, &total_freq, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if(my_rank==0) printf("Frequency of %d is %d\n", key, total_freq);

	MPI_Finalize();
}
