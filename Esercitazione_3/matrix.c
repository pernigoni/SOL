#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define dimN 16
#define dimM 8

#define CHECK_PTR_EXIT(p, s)        \
      if(p == NULL)                 \
      {                             \
            perror(#s);             \
            exit(EXIT_FAILURE);     \
      }

#define ELEM(M, i, j) M[i * dimM + j]

#define PRINTMAT(M, dimN, dimM)                                                    \
      printf("Stampo la matrice %s: \n", #M);         \
      for(int i = 0; i < dimN; i++)                   \
      {                                               \
            for(int j = 0; j < dimM; j++)             \
                  printf("%4ld ", ELEM(M, i, j));     \
            printf("\n");                             \
      }                                                        

int main() 
{
      long *M = malloc(dimN * dimM * sizeof(long));
      CHECK_PTR_EXIT(M, "malloc"); 
      
      for(size_t i = 0; i < dimN; ++i)
	      for(size_t j = 0; j < dimM; ++j)			
	            ELEM(M, i, j) = i + j;

      PRINTMAT(M, dimN, dimM);
      free(M);
      return 0;
}

/*
      i * dimM + j            dimM = 4

      i     j     indice array
      0     0     0
      0     1     1
      0     2     2
      0     3     3
      1     0     4
      1     1     5
      1     2     6
      1     3     7
      2     0     8
      2     1     9
      2     2     10
      2     3     11
*/