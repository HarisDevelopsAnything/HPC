#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>

struct Student {
    char name[100];
    char roll[8];
    double total;
    char grade;
};

char findGrade(double marks){
    if(marks > 90) return 'O';
    if(marks > 80) return 'A';
    if(marks > 70) return 'B';
    if(marks > 60) return 'C';
    if(marks > 50) return 'D';
    return 'F';
}

int main() {
    int n= 12;
    int comm_sz, rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int sub_len = n/comm_sz;

    MPI_Datatype MPI_STUDENT;

    int blocklengths[4] = {100,8,1,1};
    MPI_Datatype types[4] = {MPI_CHAR, MPI_CHAR, MPI_DOUBLE, MPI_CHAR};
    MPI_Aint offsets[4];

    offsets[0] = offsetof(struct Student, name);
    offsets[1] = offsetof(struct Student, roll);
    offsets[2] = offsetof(struct Student, total);
    offsets[3] = offsetof(struct Student, grade);

    MPI_Type_create_struct(4, blocklengths, offsets, types, &MPI_STUDENT);

    MPI_Type_commit(&MPI_STUDENT);

    struct Student* students = NULL;
    struct Student* local_students = (struct Student*)malloc(sizeof(struct Student)*sub_len);

    if(rank==0){
        FILE* fp= fopen("students", "r");
        if(!fp){
            printf("Error reading file!\n");
            MPI_Finalize();
            exit(0);
        }
        struct Student curr;
        students = (struct Student*)malloc(sizeof(struct Student)*10);
        int i=0;
        while(fscanf(fp, "%s %s %lf", curr.name, curr.roll, &curr.total) == 3){
            printf("Read %s data\n", curr.name);
            students[i] = curr;
            i++;
        }
    }

    MPI_Scatter(students, sub_len, MPI_STUDENT, local_students, sub_len, MPI_STUDENT, 0, MPI_COMM_WORLD);
    
    for(int i= 0; i<sub_len; i++){
        local_students[i].grade = findGrade(local_students[i].total);
    }

    MPI_Gather(local_students, sub_len, MPI_STUDENT, students, sub_len, MPI_STUDENT, 0, MPI_COMM_WORLD);

    if(rank == 0){
        FILE* fp = fopen("grades", "w");
        for(int i=0; i<n; i++){
            fprintf(fp, "%s %s %.2f %c\n", students[i].name, students[i].roll, students[i].total, students[i].grade);
        }
        fclose(fp);
    }
    MPI_Finalize();
}
