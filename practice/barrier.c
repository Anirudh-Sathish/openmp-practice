#include<stdio.h>
#include<mpi.h>

int main(int argc , char** argv){
    int processRank, clusterSize;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&processRank);
    for(int i =0 ; i < clusterSize; i++){
        if(i == processRank){
            printf("Hello from %d of %d \n",processRank,clusterSize);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}