#include<stdio.h>
#include<mpi.h>
#include<time.h>
#include<stdlib.h>

int main(int argc, char** argv){
    int rank, clusterSize;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int maxSize = 100;
    int arr[maxSize];
    int numbers;
    if(rank ==0){
        for(int i = 0 ; i <maxSize;i++){
            arr[i] = i;
        }
        srand(time(NULL));
        numbers = (rand()/(float)RAND_MAX)*maxSize;
        MPI_Send(arr,numbers,MPI_INT,1,0,MPI_COMM_WORLD);
        printf("0 sent %d numbers to 1\n", numbers);
    }
    else if(rank == 1){
        MPI_Status status;
        MPI_Recv(arr,maxSize,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        MPI_Get_count(&status,MPI_INT,&numbers);
        printf("1 recieved %d numbers from 0. Message source= %d, tag= %d \n",
        numbers,status.MPI_SOURCE, status.MPI_TAG);
    }
    MPI_Finalize();
    return 0;

}