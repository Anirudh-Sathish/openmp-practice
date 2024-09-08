/*
Write a n program to implement matrix multiplication
with -n 4 and verify the output.
Currently supports matmul with dimensions of any size(odd and even both supported)
and number of processes being 4

Algorithm flow
1. Init matrix A, B in  P0
2. Broadcast A, B 
3. Calculate the quadrant values
4. Gather all the quadrant values in P0
5. Store it in the final result
*/

#include<stdio.h>
#include<mpi.h>
#include<time.h>
#include<stdlib.h>
#include<assert.h>
#define MAX 20

// function to create a matrix using random integer values
// given the dimensions of the matrix
int *createMatrix(int m , int n){
    int* matrix = (int*)malloc(sizeof(int)*m*n);
    assert(matrix != NULL);
    for(int i = 0 ; i < m ; i++){
        for(int j = 0 ; j <n; j++){
            matrix[i*n+j] = (rand()%MAX)+1;
        }
    }
    return matrix;
}

// function to print a matrix
void printMatrix(int *matrix, int m, int n){
    for(int i = 0 ; i < m; i++){
        for(int j = 0; j < n; j++){
            printf("%d ",matrix[i*n+j]);
        }
        printf("\n");
    }
}

// function to perform brute matrix multiplication
int *matmul(int *matA, int* matB, int row_a, int col_a, int row_b, int col_b){
    int *result = (int*)malloc(sizeof(int)*row_a*col_b);
    for(int i = 0 ; i < row_a ; i++){
        for(int j = 0; j < col_b; j++){
            result[i*col_b + j] = 0;
        }
    }
    for(int i = 0 ; i < row_a ; i++){
        for(int j = 0; j < col_b; j++){
            for(int k = 0; k < col_a; k++){
                result[i*col_b+j] += matA[i*col_a+k] * matB[k*col_b+j];
            }
        }
    }
    return result;
}

int main(int argc , char** argv){
    if(argc != 5){
        fprintf(stderr,"Arg1: Row size of Matrix A, Arg2: Column size of Matrix A\nArg3: Row size of Matrix B, Arg4: Column size of Matrix B\n");
        exit(1);
    }

    int row_a = atoi(argv[1]);
    int col_a = atoi(argv[2]);
    int row_b = atoi(argv[3]);
    int col_b = atoi(argv[4]);

    if(col_a != row_b){
        fprintf(stderr,"Size does not support matrix multiplication\n");
        exit(1);
    }
    srand(time(NULL));
    MPI_Init(NULL, NULL);
    int rank, clusterSize;
    MPI_Comm_size(MPI_COMM_WORLD, &clusterSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(clusterSize!= 4){
        fprintf(stderr,"Support only for process count = 4 \n");
        MPI_Finalize();
        exit(1);
    }
    
    int *matrixA = NULL, *matrixB = NULL;
    int sizeA = row_a * col_a;
    int sizeB = row_b * col_b;
    if(rank == 0){
        matrixA = createMatrix(row_a, col_a);
        printf("Matrix A:\n");
        printMatrix(matrixA, row_a, col_a);
        matrixB = createMatrix(row_b, col_b);
        printf("Matrix B:\n");
        printMatrix(matrixB, row_b, col_b);
    }
    if(rank != 0){
        matrixA = (int*) malloc(sizeA * sizeof(int));
        matrixB = (int*) malloc(sizeB * sizeof(int));
    }
    // Broadcast A and B
    MPI_Bcast(matrixA, sizeA, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(matrixB, sizeB, MPI_INT, 0, MPI_COMM_WORLD); 
    int subSizeA = (row_a / 2) * (col_b / 2);
    int subSizeB = (row_a / 2) * (col_b - (col_b / 2));
    int subSizeC = (row_a -(row_a /2)) * (col_b / 2);
    int subSizeD = (row_a -(row_a /2)) * (col_b - (col_b / 2));
    int *subRes;
    // Rank 0 handles the top-left quadrant
    if (rank == 0) {
        subRes = (int *)malloc(sizeof(int) * subSizeA);
        for (int i = 0; i < row_a / 2; i++) {
            for (int j = 0; j < col_b / 2; j++) {
                subRes[i * (col_b / 2) + j] = 0;
                for (int k = 0; k < col_a; k++) {
                    subRes[i * (col_b / 2) + j] += matrixA[i * col_a + k] * matrixB[k * col_b + j];
                }
            }
        }
    }
    // Rank 1 handles the top-right quadrant
    if (rank == 1) {
        subRes = (int *)malloc(sizeof(int) * subSizeB);
        for (int i = 0; i < row_a / 2; i++) {
            for (int j = 0; j < col_b - col_b / 2; j++) {
                subRes[i * (col_b - col_b / 2) + j] = 0;
                for (int k = 0; k < col_a; k++) {
                    subRes[i * (col_b - col_b / 2) + j] += matrixA[i * col_a + k] * matrixB[k * col_b + (j + col_b / 2)];
                }
            }
        }
    }
    // Rank 2 handles the bottom-left quadrant
    if (rank == 2) {
        subRes = (int *)malloc(sizeof(int) * subSizeC);
        for (int i = 0; i < row_a - row_a / 2; i++) {
            for (int j = 0; j < col_b / 2; j++) {
                subRes[i * (col_b / 2) + j] = 0;
                for (int k = 0; k < col_a; k++) {
                    subRes[i * (col_b / 2) + j] += matrixA[(i + row_a / 2) * col_a + k] * matrixB[k * col_b + j];
                }
            }
        }
    }
    // Rank 3 handles the bottom-right quadrant
    if (rank == 3) {
        subRes = (int *)malloc(sizeof(int) * subSizeD);
        for (int i = 0; i < row_a - row_a / 2; i++) {
            for (int j = 0; j < col_b - col_b / 2; j++) {
                subRes[i * (col_b - col_b / 2) + j] = 0;
                for (int k = 0; k < col_a; k++) {
                    subRes[i * (col_b - col_b / 2) + j] += matrixA[(i + row_a / 2) * col_a + k] * matrixB[k * col_b + (j + col_b / 2)];
                }
            }
        }
    }   
    // Gather the submatrices at rank 0
    int *resultMatrix = NULL, *finalResult = NULL;
    if(rank == 0){
        resultMatrix = (int*)malloc(sizeof(int) * row_a * col_b);
        finalResult = (int*)malloc(sizeof(int) * row_a * col_b);
    }

    // set sizes and displacements
    int storeCount[4] = {subSizeA, subSizeB, subSizeC, subSizeD};
    int displacement[4] = {0, subSizeA, subSizeA + subSizeB, subSizeA + subSizeB + subSizeC};
    MPI_Gatherv(subRes,storeCount[rank],MPI_INT,resultMatrix,storeCount,displacement,MPI_INT,0,MPI_COMM_WORLD);
    int *originalResult;
    if(rank == 0){
        int index = 0;
        for(int i = 0; i < row_a / 2; i++){
            for(int j = 0; j < col_b / 2; j++){
                finalResult[i * col_b + j] = resultMatrix[index++];
            }
        }
        for(int i = 0; i < row_a / 2; i++){
            for(int j = col_b / 2; j < col_b; j++){
                finalResult[i * col_b + j] = resultMatrix[index++];
            }
        }
        for(int i = row_a / 2; i < row_a; i++){
            for(int j = 0; j < col_b / 2; j++){
                finalResult[i * col_b + j] = resultMatrix[index++];
            }
        }
        for(int i = row_a / 2; i < row_a; i++){
            for(int j = col_b / 2; j < col_b; j++){
                finalResult[i * col_b + j] = resultMatrix[index++];
            }
        }

        printf("Parallel Matmul Result Matrix:\n");
        printMatrix(finalResult,row_a,col_b);
        originalResult = matmul(matrixA, matrixB, row_a, col_a, row_b, col_b);
        printf("Sequential Matmul Result Matrix:\n");
        printMatrix(originalResult,row_a,col_b);
        // checking for correctness
        for(int i = 0; i < row_a; i++){
            for(int j = 0; j < col_b; j++){
                assert(originalResult[i * col_b + j]==finalResult[i * col_b + j]);
            }
        }
    }
    if(rank == 0){
        free(matrixA);
        free(matrixB);
        free(resultMatrix);
        free(finalResult);
        free(originalResult);
    }
    free(subRes);
    MPI_Finalize();
    return 0;
}