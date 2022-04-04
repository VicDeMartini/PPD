#!/bin/bash
gcc escalabilidade.c -fopenmp 
export OMP_NUM_THREADS=6
echo "Número de Threads =  $OMP_NUM_THREADS"
time ./a.out
echo

export OMP_NUM_THREADS=12
echo "Número de Threads =  $OMP_NUM_THREADS"
time ./a.out
echo

export OMP_NUM_THREADS=24
echo "Número de Threads =  $OMP_NUM_THREADS"
time ./a.out
echo
