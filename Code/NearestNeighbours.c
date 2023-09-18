#include "NearestNeighbours.h"
#include "BoundedPriorityQueue.h"
#include "DynamicTimeWarping.h"
#include "math.h"
#include <stdlib.h>//for malloc

SketchDistance* nearestNeighbours(const Dataset* dataset, Sketch query, size_t k) {
  //On crée une file à priorité de capacité k 
  SketchDistance* sd = malloc(k*sizeof(SketchDistance)); 
  if(!sd){
    printf("Allocation of sd failed\n");
    return NULL;
  }
  BoundedPriorityQueue* bpq = bpqCreate(k);
  
  //On parcourt le data set on remplit la bpq tant qu'elle n'est pas full
  //Si elle est full, on remplace son pire élément par l'élément courant si celui-ci est meilleur
  size_t ds_len = dataset->size;

  for (size_t i=0; i<ds_len; i++){

    if(bpqSize(bpq) < bpqCapacity(bpq)){//queue is not full
      Sketch current_sketch = dataset->sketches[i];
      double current_distance = dtw(query,current_sketch,INFINITY);
      if(!bpqInsert(bpq, current_distance, i)){
        printf("could not insert dude\n");
        return NULL;        
      }

    }
    else{ //queue is full
      double worst_distance_of_k = bpqMaximumKey(bpq);
      Sketch current_sketch = dataset->sketches[i];
      double current_distance = dtw(query,current_sketch, worst_distance_of_k);
      if(current_distance < worst_distance_of_k){
        bpqReplaceMaximum(bpq, current_distance,i);
      }
    }
    
  } 

  for(size_t i=0; i<k; i++){
    size_t* values = bpqGetItems(bpq);
    Sketch *current_sketch = dataset->sketches+values[i];
    sd[i].sketch = current_sketch;
    sd[i].distance = dtw(query,*current_sketch,100000.0) ;
  }
 
  bpqFree(bpq);
  return sd;
}