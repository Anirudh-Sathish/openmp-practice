#include<stdio.h>
#include<mpi.h>

int main(int argc, char** argv){
    int rank, clusterSize;
    MPI_Init(&argc, &argv);
    int limit = 20;
    int pingCount = 0;
    MPI_Comm_size(MPI_COMM_WORLD,&clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(clusterSize != 2){
        printf("Only cluster size of 2 accepted");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    int partnerRank = (rank+1)%2;
    while(pingCount < limit){
        if(rank == pingCount%2){
            pingCount++;
            MPI_Send(&pingCount,1,MPI_INT,partnerRank,0,MPI_COMM_WORLD);
            printf("%d sent and incremented ping_pong_count "
               "%d to %d\n", rank, pingCount,partnerRank);
        }else {
            MPI_Recv(&pingCount,1,MPI_INT,partnerRank,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            printf("%d received ping_pong_count %d from %d\n",
               rank, pingCount, partnerRank);
        }
    }
    MPI_Finalize();
    return 0;
}