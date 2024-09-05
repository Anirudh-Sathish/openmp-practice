/*
Write a n program to implement sum reduction  with -n 4 and
verify the output. Do this for large lists 
Currently supports array sizes,
that are a multiple of the number of processes
Currently supports array sizes uptill 10,00,000
*/
#include<stdio.h>
#include<mpi.h>
#include<time.h>
#include<stdlib.h>
#include <assert.h>

// generates a random integer vector, given the size of the vector
int *genreateRandomVector(int vecSize){
    int* vec = (int*)malloc(sizeof(int)*vecSize);
    assert(vec!=NULL);
    for(int i = 0; i <vecSize; i++){
        vec[i] = rand()%vecSize;
    }
    return vec;
}

// Reduces an array, into a single number, by performing summation 
// on the array
int applyReduction(int* vec1, int vecSize){
    int result =0;
    for(int i = 0 ; i< vecSize; i++){
        result+= vec1[i];
    }
    return result;
}

int main(int argc, char **argv){
    if(argc !=2){
        fprintf(stderr,"Enter the size of the vectors \n");
        exit(1);
    }
    int vecSize = atoi(argv[1]);
    srand(time(NULL));
    int *vec1;
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
    }
    //  prepare for scatter
    int elementSize = vecSize/clusterSize;
    int *subVec1 = (int*)malloc(sizeof(int)*elementSize);
    MPI_Scatter(vec1,elementSize,MPI_INT,subVec1,elementSize,MPI_INT,0,MPI_COMM_WORLD);
    int subResult = applyReduction(subVec1,elementSize);
    int result, originalResult;
    int *resultVector;
    if(rank ==0){
        resultVector = (int*)malloc(sizeof(int)*vecSize);
        assert(resultVector!=NULL);
    }
    MPI_Gather(&subResult, 1, MPI_INT, resultVector, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank ==0){
        printf("Print resultant vector after reduction: \n");
        for(int i = 0 ; i <clusterSize; i++){
            printf(" %d ",resultVector[i]);
        }
        printf("\n");
        result = applyReduction(resultVector,clusterSize);
        printf("Result after openMP Reduction: %d \n",result);
        originalResult = applyReduction(vec1,vecSize);
        printf("Original Reduction Result: %d \n",originalResult);
    }
    // cleanup
    if(rank ==0){
        free(vec1);
        free(resultVector);
    }
    free(subVec1);
    MPI_Finalize();
    return 0;
}