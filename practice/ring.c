#include<stdio.h>
#include<mpi.h>

int main(int argc, char** argv){
    int rank, clusterSize;
    MPI_Init(&argc,&argv);
    int token;
    MPI_Comm_size(MPI_COMM_WORLD,&clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(rank !=0){
        MPI_Recv(&token,1,MPI_INT,rank-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n",
           rank, token, rank - 1);
    }else{
        token =-1;
    }
    MPI_Send(&token,1,MPI_INT,(rank+1)%clusterSize,0,MPI_COMM_WORLD);
    if(rank ==0){
        MPI_Recv(&token,1,MPI_INT,clusterSize-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n",
           rank, token, clusterSize-1);
    }
}