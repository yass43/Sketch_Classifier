#include "Sketch.h"
#include "DynamicTimeWarping.h"
#include <math.h>
#include <stdlib.h>//for malloc

static double abs_distance(Point a, Point b);
static double min3(double a, double b, double c);

double dtw(Sketch sketch1, Sketch sketch2, double maxDistance) {
  //On commence par calculer la matrice de coût C(i,j) dont l'élément c_{i,j} = d(Sketch1_i,Sketch2_j)
  //où Sketch1_i correspond à la i-ème valeur (feature value) du Sketch1, i.e, la position de son i-ème point, i-ème entrée

    size_t s1_len = sketch1.size;
    size_t s2_len = sketch2.size;
    double* matrix_C = malloc((s1_len * s2_len) * sizeof(double));
    for(size_t i = 0; i < s1_len; i++){
      for(size_t j = 0; j < s2_len; j++){
        matrix_C[i*s2_len+j]=abs_distance(sketch1.points[i], sketch2.points[j]);
        //matrix_C[i*s2_len] = abs_distance(sketch1.points[i], sketch2.points[0]);
        //matrix_C[j] = abs_distance(sketch1.points[0], sketch2.points[j])
      }
    }
  //On définit la matrice de coût accumulé optimal D et on l'initialise 
  double* matrix_D = malloc((s1_len * s2_len) * sizeof(double));
  for(size_t i = 0; i < s1_len; i++){
      for(size_t j = 0; j < s2_len; j++){
        matrix_D[i*s2_len+j] = 0.0;
      }
  }
  //On initialise le point de départ et le point d'arrivée.
  matrix_D[s1_len*s2_len-1]=abs_distance(sketch1.points[s1_len-1],sketch2.points[s2_len-1]);//(N,M)
  matrix_D[0]=abs_distance(sketch1.points[0],sketch2.points[0]);//(0,0)
  //On initialise la frontière bas et gauche : 
  for(size_t i = 1; i < s1_len; i++){
    matrix_D[i*s2_len] = matrix_D[(i-1)*s2_len] + matrix_C[i*s2_len]; 
  }
  for(size_t j = 1; j < s2_len; j++){
    matrix_D[j] = matrix_D[j-1] + matrix_C[j]; /
  }
  //On remplit D
  double min=matrix_D[0];
  for(size_t i = 1; i < s1_len; i++){
      for(size_t j = 1; j < s2_len; j++){
        matrix_D[i*s2_len+j] = matrix_C[i*s2_len+j] + min3(matrix_D[(i-1)*s2_len+j], matrix_D[i*s2_len+(j-1)], matrix_D[(i-1)*s2_len+(j-1)]);
        //implement stop fast here with a double break
        if(matrix_D[i*s2_len+j] <= min)
          min = matrix_D[i*s2_len+j];

      }
      if(min >=maxDistance)
        return INFINITY;
    }

  double dtw_distance =  matrix_D[s1_len*s2_len-1];
  free(matrix_C);
  free(matrix_D);
  return dtw_distance;
}

static double abs_distance(Point a, Point b){
  double distance = 0.5*(abs(a.x - b.x) + abs(a.y - b.y));
  return distance;
}

static double min3(double a, double b, double c){
  double min =0.0;
  if( a<b && a<c){
    min = a;
  }
  else if(b<c){
    min = b;
  }
  else{
    min = c;
  }
  return min;
}

