#include<stdio.h>
#include<mpi.h>

int main(int argc, char** argv){
    int pRank, clusterSize;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&pRank);
    int number;
    if(pRank ==0){
        number = -1;
        MPI_Send(&number,1,MPI_INT,1,0,MPI_COMM_WORLD);
    } else if (pRank ==1){
        MPI_Recv(&number,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("Process 1 received number %d from process 0\n",
           number);
        
    }

    MPI_Finalize();
    return 0;
}