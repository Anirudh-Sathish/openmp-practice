/*
Write a vector addition programs with -n 4 and
verify the output
Supports different types of array sizes
*/
#include<stdio.h>
#include<mpi.h>
#include<time.h>
#include<assert.h>
#include<stdlib.h>

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

int main(int argc, char** argv){
    if(argc !=2){
        fprintf(stderr,"Enter number of elements \n");
        exit(1);
    }
    int rank, clusterSize;
    int vecSize = atoi(argv[1]);
    srand(time(NULL));
    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    // generate arrays
    int *vec1, *vec2;
    int baseSize = vecSize/clusterSize;
    int remainder = vecSize%clusterSize;
    int *sendCount = (int*)malloc(sizeof(int)*clusterSize);
    int *displacements = (int*)malloc(sizeof(int)*clusterSize);

    // set sizes and displacements
    int currDisplacement = 0;
    for(int i = 0 ; i< clusterSize;i++){
        sendCount[i] = baseSize +(i < remainder ? 1 : 0);
        displacements[i] = currDisplacement;
        currDisplacement+= sendCount[i];
    }
    if(rank == 0){
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

    int localSize = sendCount[rank];
    int *subVec1 = (int*)malloc(sizeof(int)*localSize);
    int *subVec2 = (int*)malloc(sizeof(int)*localSize);

    // Scatter to variable number
    MPI_Scatterv(vec1, sendCount, displacements, MPI_INT, subVec1, localSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(vec2, sendCount, displacements, MPI_INT, subVec2, localSize, MPI_INT, 0 , MPI_COMM_WORLD);

    int *resSubVec = addVectors(subVec1,subVec2,localSize);
    int *resultVector = NULL;
    if(rank == 0){
        resultVector = (int*)malloc(sizeof(int)*vecSize);
        assert(resultVector != NULL);
    }
    MPI_Gatherv(resSubVec,localSize,MPI_INT,resultVector,sendCount,displacements,MPI_INT,0,MPI_COMM_WORLD);
    int *originalResult;
    if(rank == 0){
        printf("Print resultant vector after addition: \n");
        for(int i = 0 ; i <vecSize; i++){
            printf(" %d ",resultVector[i]);
        }
        printf("\n");

        originalResult = addVectors(vec1, vec2, vecSize);
        printf("Print resultant vector after addition without mpi: \n");
        for(int i = 0 ; i <vecSize; i++){
            printf(" %d ",originalResult[i]);
        }
        printf("\n");

        // Assert that they are the same
        for(int i = 0 ; i < vecSize; i++){
            assert(resultVector[i] == originalResult[i]);
        }
    }

    // clear the space
    if(rank == 0){
        free(vec1);
        free(vec2);
        free(resultVector);
        free(originalResult);
    }

    free(subVec1);
    free(subVec2);
    free(resSubVec);
    free(sendCount);
    free(displacements);

    MPI_Finalize();
    return 0;
}