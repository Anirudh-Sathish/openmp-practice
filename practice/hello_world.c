#include<stdio.h>
#include<mpi.h>

int main(int argc , char** argv){
        int processRank, clusterSize;
        char processorName[MPI_MAX_PROCESSOR_NAME];
        int strLen;
        MPI_Init(&argc,&argv);
        MPI_Comm_size(MPI_COMM_WORLD, &clusterSize);
        MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
        MPI_Get_processor_name(processorName,&strLen);
        printf("\n Hello world from processor %s containing %d of %d \n",processorName,processRank, clusterSize);
        MPI_Finalize();
        return 0;
}