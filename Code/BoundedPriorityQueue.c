#include "BoundedPriorityQueue.h"
#include <stdlib.h>
#include <stdio.h>

// on a rajouté: définition d'une nouvelle structure pour les éléments contenu dans bpq_l
typedef struct bounded_priority_queue_e {
  double key;
  size_t value;
} BPQElement;

struct bounded_priority_queue_t {
  BPQElement* bpq_l;
  size_t size;
  size_t capacity;
};

static void swap(BPQElement* bpq_l, int index1, int index2);
static void maxHeapfy(BPQElement *bpq_l, size_t i, size_t heap_size);
static void bpqHeapfy(BPQElement* bpq_l, size_t size);

//-----------------IMPLEMENTATION DES FONCTIONS------------------------//


BoundedPriorityQueue* bpqCreate(size_t capacity) {
  BoundedPriorityQueue* bpq = malloc(sizeof(BoundedPriorityQueue));
  bpq->bpq_l = calloc(capacity,sizeof(BPQElement)); 
  bpq->size = 0;
  bpq->capacity = capacity;
  return bpq;
}

void bpqFree(BoundedPriorityQueue* bpq) {
  free(bpq->bpq_l);
  free(bpq);
}

// on a rajouté: Simple swap permettant d'échanger 2 éléments dans la bpq_l
static void swap(BPQElement* bpq_l, int index1, int index2){
    BPQElement tmp = bpq_l[index1];
    bpq_l[index1] = bpq_l[index2];
    bpq_l[index2] = tmp;
}



// on a rajouté: maxHeapfy permet de réarranger le tas pour maintenir la propriété d'ordre du tas
static void maxHeapfy(BPQElement *bpq_l, size_t i, size_t heap_size){
  size_t largest = i;
  size_t l = 2*i;
  size_t r = 2*i+1;

  if(l < heap_size && bpq_l[l].key > bpq_l[i].key)
    largest = l;

  if(r < heap_size && bpq_l[r].key > bpq_l[largest].key)
    largest = r;

  if(largest != i){
    swap(bpq_l, i, largest);
    maxHeapfy(bpq_l, largest, heap_size);
  }
}

// on a rajouté: bpqHeapfy permet de construire un tas
static void bpqHeapfy(BPQElement* bpq_l, size_t size) {
  for(int i = size/2; i >= 0; i--){ 
    maxHeapfy(bpq_l, i, size);
  }
}

bool bpqInsert(BoundedPriorityQueue* bpq, double key, size_t value) {

  if (bpq->size >= bpq->capacity){
    return false;
  }
  else{
    bpq->bpq_l[bpq->size].key = key; 
    bpq->bpq_l[bpq->size].value = value;
    bpq->size++;
   
    bpqHeapfy(bpq->bpq_l, bpq->capacity);
    
    return true;
  }
}

void bpqReplaceMaximum(BoundedPriorityQueue* bpq, double key, size_t value) {

  bpq->bpq_l[0].key = key;
  bpq->bpq_l[0].value = value;
  bpqHeapfy(bpq->bpq_l,bpq->capacity);
}

double bpqMaximumKey(const BoundedPriorityQueue* bpq) {
  
  if (bpq->size <= 0){
    return -1;
  }
  bpqHeapfy(bpq->bpq_l,bpq->capacity);
  
  return bpq->bpq_l[0].key;
} 

size_t* bpqGetItems(const BoundedPriorityQueue* bpq) {
  if(bpq==NULL)
    return NULL;
  if(bpqSize(bpq)==0)
    return NULL;
  size_t* neighbours_index = malloc(bpq->size * sizeof(size_t));
  for(size_t i = 0; i<bpq->size; i++){
    neighbours_index[i] = bpq->bpq_l[i].value;
  }
  return neighbours_index;
}

size_t bpqSize(const BoundedPriorityQueue* bpq) {
  
  return bpq->size;
}

size_t bpqCapacity(const BoundedPriorityQueue* bpq) {
  
  return bpq->capacity;
}