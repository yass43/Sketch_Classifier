#include <math.h>
#include <stdlib.h>//for malloc
#include <stdio.h>


int main(){
    double testos = 9.5;
    size_t s1_len = 5;
    size_t s2_len = 2;
    double* matrix = malloc((s1_len * s2_len) * sizeof(double));
    for(size_t i = 0; i < s1_len; i++){
      for(size_t j = 0; j < s2_len; j++){
        matrix[i*s2_len+j]=i+j;
      }
    }
    for(size_t i = 0; i < s1_len; i++){
      for(size_t j = 0; j < s2_len; j++){
        fprintf(stdout,"hello");//fprintf(stdout,"hello") works!
        
      }
      printf("\n");
    }
    free(matrix);
  fflush(stdout);
  return 0;
}