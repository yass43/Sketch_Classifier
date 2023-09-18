#include "Sketch.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "NearestNeighbours.h"


/** ------------------------------------------------------------------------ *
 * Print the help message.
 *
 * PARAMETERS
 * programName   The name of the program
 * ------------------------------------------------------------------------- */
static void printHelp(const char* programName)
{
    fprintf(stderr, "%s %s %s\n", "Usage:", programName, "[-i item_index] "
            "[-k n_neighbours] [-o output_path] train_filepath test_filepath");
}


/** ------------------------------------------------------------------------ *
 * Save the given sketch in folder `output_path`
 *
 * PARAMETERS
 * output_path   The folder in which to save the sketch
 * sketch        The sketch to save
 * distance      The distance with the query
 * ------------------------------------------------------------------------- */
static void saveNeighbour(const char* output_path, Sketch sketch,
                          double distance) {
    char buffer[256];
    sprintf(buffer, "%s/image_%s_%5.2lf.ppm", output_path,
            getClassName(sketch.class), distance);
    sketch2ppm(sketch, buffer);
}

/** ------------------------------------------------------------------------ *
 * Comparison function for `SketchDistance`
 *
 * PARAMETERS
 * sd1           A `SketchDistance`
 * sd2           A `SketchDistance`
 *
 * RETURN
 * comp          0 if both argument are the same, < 0 if the first argument
 *               is less than the second, > 0 otherwise
 * ------------------------------------------------------------------------- */
static int sketchDistanceCompare(const void* sd1, const void* sd2) {
    double diff = (((SketchDistance*)sd1)->distance -
                   ((SketchDistance*)sd2)->distance);
    return diff < 0 ? -1 : (diff > 0 ? 1 : 0);
}


/** ------------------------------------------------------------------------ *
 * Free all arguments, according to its type
 *
 * PARAMETERS
 * trainingset   Something to free
 * testingset    Something to free
 * neighbours    Something to free
 * ------------------------------------------------------------------------- */
static void freeAll(Dataset* trainingset, Dataset* testingset,
                    SketchDistance* neighbours) {
    if(trainingset != NULL)
        freeDataset(trainingset);
    if(testingset != NULL)
        freeDataset(testingset);
    if(neighbours != NULL)
        free(neighbours);
}

/** ------------------------------------------------------------------------ *
 * Returns the elapsed time, in seconds, with respect to a starting point.
 *
 * PARAMETERS
 * start        A starting point in clock ticks
 *
 * RETURN
 * time         elapsed time, with respect to start
 * ------------------------------------------------------------------------- */
static double elapsed(clock_t start) {
    return ((double) (clock() - start)) / CLOCKS_PER_SEC;
}

/** ------------------------------------------------------------------------ *
 * Returns the majority class among a set of k neighbors (ties are broken
 * arbitrarily).
 *
 * PARAMETERS
 * neighbours    An array of neighbours
 *
 * RETURN
 * class         the index of the majority class
 * ------------------------------------------------------------------------- */
static int getMajorityClass(SketchDistance *neighbours, int k) {

  int classCount[10];
  for (int i = 0; i < 10; i++)
    classCount[i] = 0;
  for (int i = 0; i < k; i++)
    classCount[neighbours[i].sketch->class]++;
  int majorityClass = -1;
  int maxCount = -1;
  for (int i = 0; i < 10; i++)
    if (classCount[i] > maxCount) {
      majorityClass = i;
      maxCount = classCount[i];
    }
  return majorityClass;
}

/** ------------------------------------------------------------------------ *
 * Search for the k nearest neighbours of a given sketch in a given
 * reference database or compute the error rate over the full testing database.
 *
 * SYNOPSIS
 * search [-i item_index] [-k n_neighbours] [-o output_path] train_filepath
 *        test_filepath
 *
 * DESCRIPTION
 * -i item_index      The index of the test sample to use. If no index is
 *                    provided, compute the error rate over all test examples.
 * -k n_neighbours    The number of neighbours to compute (Default: 5).
 * -o output_path     The folder in which to save the sketches (Default: no save).
 * train_filepath     The path to the reference database
 * test_filepath      The path to the testing database
 * ------------------------------------------------------------------------- */
int main(int argc, char *argv[]) {

    srand(time(NULL));

    /* ------------------------------ Parsing ------------------------------ */
    if (argc < 3) {
        fprintf(stderr, "Aborting; There must be at least 2 parameters"
                        " (given %d). \n", argc-1);
        printHelp(argv[0]);
        return EXIT_FAILURE;
    }

    int index = -1;
    size_t k = 5;
    char* output_path = NULL;
    char* train_path = argv[argc-2];
    char* test_path = argv[argc-1];
    int argi = 1;

    while(argi < argc - 2) {
        if(strcmp(argv[argi], "-i") == 0) {
            size_t itemIndex;
            if(sscanf (argv[++argi], "%zu", &itemIndex) != 1) {
                fprintf(stderr, "%s\n", "Aborting; item index ('-i') should be"
                                        " an unsigned integer.");
                return EXIT_FAILURE;
            }
            index = itemIndex;
        } else if(strcmp(argv[argi], "-k") == 0) {
            size_t numberOfNeigbours;
            if(sscanf (argv[++argi], "%zu", &numberOfNeigbours) != 1) {
                fprintf(stderr, "%s\n", "Aborting; number of neighbours ('-k')"
                                        " should be an unsigned integer.");
                return EXIT_FAILURE;
            }
            k = numberOfNeigbours;
        } else if(strcmp(argv[argi], "-o") == 0) {
	    output_path = argv[++argi];
        } else {
            fprintf(stderr, "%s '%s'\n", "Aborting; unknown parameter",
                    argv[argi]);
            printHelp(argv[0]);
            return EXIT_FAILURE;
        }
        argi++;
    }


    Dataset* trainingset = NULL;
    Dataset* testingset = NULL;
    SketchDistance* neighbours = NULL;

    /* --------------------------- Load sketches --------------------------- */
    trainingset = loadDataset(train_path, stdout);
    if(!trainingset)
    {
        fprintf(stderr, "Aborting; could not load training set (%s)\n",
                train_path);
        freeAll(trainingset, testingset, neighbours);
        return EXIT_FAILURE;
    }
    testingset = loadDataset(test_path, stdout);
    if(!testingset)
    {
        fprintf(stderr, "Aborting; could not load testing set (%s)\n",
                test_path);
        freeAll(trainingset, testingset, neighbours);
        return EXIT_FAILURE;
    }

    if (index >= 0) { 
      /* ---------------- Single nearest neighbours search ------------------*/
      index %= testingset->size;  // to make sure we are in the range
      
      printf("Class of selected sketch %d is %s (from test set)\n", index,
	     getClassName(testingset->sketches[index].class));
      //printf("before neighbours call\n");
      neighbours = nearestNeighbours(trainingset, testingset->sketches[index], k);
      //printf("after neighbours call");
      if(!neighbours)
      {
	  fprintf(stderr, "%s\n", "Aborting, could not find the neighbours.");
	  freeAll(trainingset, testingset, neighbours);
	  return EXIT_FAILURE;
      }

      qsort(neighbours, k, sizeof(SketchDistance), sketchDistanceCompare);

      printf("Neighbours (from training set): \n");
      for(size_t i = 0; i < k; ++i) {
        printf("%zu) class '%s' with distance %lf\n", i + 1,
               getClassName(neighbours[i].sketch->class),
	       neighbours[i].distance);
	if (output_path)
	  saveNeighbour(output_path,
			*(neighbours[i].sketch),
			neighbours[i].distance);
      }
    
      if (output_path) {
	char buffer[256];
	sprintf(buffer, "%s/query.ppm", output_path);
	sketch2ppm(testingset->sketches[index], buffer); 
      }
      freeAll(trainingset, testingset, neighbours);
    } else {
      /* ----------- Compute the error rate over the test set --------------*/
      size_t nbErrors = 0;

      fprintf(stdout,"Testing");
      clock_t start = clock();
      for (size_t i = 0; i < testingset->size; i++) {
	neighbours = nearestNeighbours(trainingset, testingset->sketches[i], k);
	if(!neighbours)
	{
	   fprintf(stderr, "%s\n", "Aborting, could not find the neighbours.");
	   freeAll(trainingset, testingset, neighbours);
	   return EXIT_FAILURE;
        }
	int predictedClass = getMajorityClass(neighbours, k);
	if (predictedClass != testingset->sketches[i].class)
	  nbErrors++;
	free(neighbours);
	printf(".");
	fflush(stdout);
      }
      printf("\nError rate on the testing database: %.2f(%%) (%zu examples misclassified)\n",
	     (float)nbErrors/testingset->size*100, nbErrors);
      printf("Error rate computed in %f seconds.\n", elapsed(start));
      
      freeAll(trainingset, testingset, NULL);      
    }

    return EXIT_SUCCESS;
}

