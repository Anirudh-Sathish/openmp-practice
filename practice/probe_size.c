#include<stdio.h>
#include<mpi.h>
#include<time.h>
#include<stdlib.h>

int main(int argc, char** argv){
    int rank, clusterSize;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int numbers;
    if(rank ==0){
        int maxSize = 100;
        int arr[maxSize] = {-1};
        srand(time(NULL));
        numbers = (rand()/(float)(RAND_MAX))*maxSize;
        MPI_Send(arr,numbers,MPI_INT,1,0,MPI_COMM_WORLD);
        printf("0 sent %d numbers to 1\n", numbers);
    }else if(rank ==1){
        MPI_Status status;
        MPI_Probe(0,0,MPI_COMM_WORLD,&status);
        MPI_Get_count(&status,MPI_INT,&numbers);
        int *buf = (int*)malloc(sizeof(int)*numbers);
        MPI_Recv(buf,numbers,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("1 dynamically received %d numbers from 0.\n",
           numbers);
        free(buf);
    }
    MPI_Finalize();
    return 0;
}