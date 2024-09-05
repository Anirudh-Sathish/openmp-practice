/*
Write a vector addition programs with -n 4 and
verify the output
Currently supports array sizes,
that are a multiple of the number of processes
*/
#include<stdio.h>
#include<mpi.h>
#include<time.h>
#include<stdlib.h>
#include <assert.h>

int *genreateRandomVector(int vecSize){
    int* vec = (int*)malloc(sizeof(int)*vecSize);
    assert(vec!=NULL);
    for(int i = 0; i <vecSize; i++){
        vec[i] = rand()%vecSize;
    }
    return vec;
}
int *addVectors(int* vec1, int *vec2, int vecSize){
    int *resVec = (int*)malloc(sizeof(int)*vecSize);
    assert(resVec!=NULL);
    for(int i = 0 ; i< vecSize; i++){
        resVec[i] = vec1[i]+vec2[i];
    }
    return resVec;
}

int main(int argc, char **argv){
    if(argc !=2){
        fprintf(stderr,"Enter the size of the vectors \n");
        exit(1);
    }
    int vecSize = atoi(argv[1]);
    srand(time(NULL));
    int *vec1, *vec2;
    int rank, clusterSize;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(vecSize%clusterSize!=0){
        fprintf(stderr,"Number of process should be multiple of size, currently only this is supported\n");
        MPI_Finalize();
        exit(1);
    }

    if(rank ==0){
        vec1 = genreateRandomVector(vecSize);
        printf("Print vec1: \n");
        for(int i = 0 ; i <vecSize; i++){
            printf(" %d ",vec1[i]);
        }
        printf("\n");
        vec2 = genreateRandomVector(vecSize);
        printf("Print vec2: \n");
        for(int i = 0 ; i <vecSize; i++){
            printf(" %d ",vec2[i]);
        }
        printf("\n");
    }
    //  prepare for scatter
    int elementSize = vecSize/clusterSize;
    int *subVec1 = (int*)malloc(sizeof(int)*elementSize);
    int *subVec2 = (int*)malloc(sizeof(int)*elementSize);
    MPI_Scatter(vec1,elementSize,MPI_INT,subVec1,elementSize,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatter(vec2,elementSize,MPI_INT,subVec2,elementSize,MPI_INT,0,MPI_COMM_WORLD);
    int *resSubVec = addVectors(subVec1,subVec2,elementSize);
    int *resultVector, *originalResult;
    if(rank ==0){
        resultVector = (int*)malloc(sizeof(int)*vecSize);
        assert(resultVector!=NULL);
    }
     MPI_Gather(resSubVec, elementSize, MPI_INT, resultVector, elementSize, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank ==0){
        printf("Print resultant vector after addition: \n");
        for(int i = 0 ; i <vecSize; i++){
            printf(" %d ",resultVector[i]);
        }
        printf("\n");
        originalResult = addVectors(vec1,vec2,vecSize);
        printf("Print resultant vector after addition without mpi: \n");
        for(int i = 0 ; i <vecSize; i++){
            printf(" %d ",originalResult[i]);
        }
        printf("\n");

        // assert that they are the same
        for(int i = 0 ; i < vecSize;i++){
            assert(resultVector[i] = originalResult[i]);
        }
    }
    // cleanup
    if(rank ==0){
        free(vec1);
        free(vec2);
        free(resultVector);
        free(originalResult);
    }
    free(subVec1);
    free(subVec2);
    free(resSubVec);
    MPI_Finalize();
    return 0;
}