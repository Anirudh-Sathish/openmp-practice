#include<stdio.h>
#include<mpi.h>
#include<time.h>
#include<stdlib.h>
#include <assert.h>

int *generate_array(int size){
    int *arr = (int*)malloc(sizeof(int)*size);
    assert(arr!=NULL);
    for(int i = 0 ; i<size; i++){
        arr[i] = i;
    }
    return arr;
}

float get_average(int *arr, int arrSize){
    float avg = 0.f;
    for(int i = 0 ;  i < arrSize; i++){
        avg+=arr[i];
    }
    return (avg/arrSize);
}
float get_average_float(float *arr, int arrSize){
    float avg = 0.f;
    for(int i = 0 ;  i < arrSize; i++){
        avg+=arr[i];
    }
    return (avg/arrSize);
}
int main(int argc, char** argv){
    if(argc!=2){
        fprintf(stderr,"Provide number of elements");
        exit(1);
    }
    int size = atoi(argv[1]);
    
    // create random array with mentioned size
    int *arr1 = NULL, *arr2 = NULL;
    MPI_Init(NULL,NULL);
    int rank, clusterSize;
    MPI_Comm_size(MPI_COMM_WORLD,&clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(size%clusterSize!=0){
        fprintf(stderr,"Number of process should be multiple of size, currently only this is supported\n");
        exit(1);
    }

    if(rank ==0){
        arr1 = generate_array(size);
    }
    int elementSize = size/clusterSize;
    int *subArr = (int*)malloc(sizeof(int)*elementSize);
    assert(subArr!=NULL);
    MPI_Scatter(arr1,elementSize,MPI_INT,subArr,elementSize,MPI_INT,0,MPI_COMM_WORLD);
    float sub_avg = get_average(subArr,elementSize);
    
    float *accum_avg;
    if(rank ==0){
        accum_avg = (float*)malloc(sizeof(float)*clusterSize);
        assert(accum_avg!=NULL);
    }
    MPI_Gather(&sub_avg,1,MPI_FLOAT, accum_avg,1,MPI_FLOAT,0,MPI_COMM_WORLD);
    if(rank ==0){
        float avg = get_average_float(accum_avg,clusterSize);
        printf("\nAvg of all elements is %f\n", avg);
        float original_data_avg =get_average(arr1, size);
        printf("Avg computed across original data is %f\n", original_data_avg);
    }

    // clean up
    if(rank ==0){
        free(arr1);
        free(accum_avg);
    }
    free(subArr);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}