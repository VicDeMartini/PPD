/*
Trabalho estudo do pararelismo da multiplicação de matrizes

Esse trabalho tem o inttuiro de investigar a escalabilidade do programa de 
multiplicação de matrizes utilizando OpenMP.

Aluna:
Victoria de Martini de Souza - 759378

*/
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

float *A, *B, *C;

int main(int argc, char *argv[])
{
    int lin_a, col_a, lin_b, col_b, lin_c, col_c;
    int i, j, k;

    lin_b = col_a = 1000;
    lin_c = lin_a = 1000;
    col_c = col_b = 1000;

    // Alocacao dinamica das matrizes, com linhas em sequencia
    A = (float *)malloc(lin_a * col_a * sizeof(float));
    B = (float *)malloc(lin_b * col_b * sizeof(float));
    C = (float *)malloc(lin_c * col_c * sizeof(float));

    // Atribucao de valores iniciais as matrizes
    srandom(time(NULL));

    for (i = 0; i < lin_a * col_a; i++)
        A[i] = (float)rand() / (float)RAND_MAX;

    for (i = 0; i < lin_b * col_b; i++)
        B[i] = (float)rand() / (float)RAND_MAX;

    // calculo da multiplicacao

    // Qual/quais loop(s) paralelizar? Vale a pena paralelizar todos?
    // Qual é o efeito de fazer um parallel for em cada um dos fors abaixo?
    // É necessários sincronizar alguma operação, garantindo exclusão mútua?

    // Medicao de tempo do bloco de fors
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    int soma = 0;

    #pragma omp parallel for
    for (i = 0; i < lin_c; i++) // loop externo
        for (j = 0; j < col_c; j++){ // loop intermediario
            soma = 0;
            for (k = 0; k < col_a; k++) // loop interno
                soma += A[i * col_a + k] * B[k * col_b + j];
            C[i * col_c + j] = soma;
        }

    gettimeofday(&tv2, NULL);

    printf("Total linhas a: %d\n", lin_a);

    printf("Total colunas a / linhas b: %d\n", (col_a/lin_b));

    printf("Total colunas b: %d\n", col_b);

    printf("Total time = %f seconds\n",
           (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 +
               (double)(tv2.tv_sec - tv1.tv_sec));

    return 0;
}
