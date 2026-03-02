#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

int isPalindrome(char str[]) {
    int i = 0, j = strlen(str) - 1;
    while (i < j) {
        if (str[i] != str[j])
            return 0;
        i++;
        j--;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int rank, size;
    char msg[MAX];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Recv(msg, MAX, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == 1) {
                for (int k = 0; msg[k]; k++)
                    msg[k] = toupper(msg[k]);

                printf("Received from rank %d (Type 1): Uppercase = %s\n", i, msg);
            } 
            else if (status.MPI_TAG == 2) {
                if (isPalindrome(msg))
                    printf("Received from rank %d (Type 2): \"%s\" is a Palindrome\n", i, msg);
                else
                    printf("Received from rank %d (Type 2): \"%s\" is NOT a Palindrome\n", i, msg);
            }
        }
    } 
    else {
        if (rank % 2 == 1) {
            strcpy(msg, "hello mpi");
            MPI_Send(msg, strlen(msg) + 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        } 
        else {            
            strcpy(msg, "madam");
            MPI_Send(msg, strlen(msg) + 1, MPI_CHAR, 0, 2, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
