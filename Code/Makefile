# Macros
CC = gcc
CFLAGS = -std=c99 --pedantic -Wall -Wextra -Wmissing-prototypes

ALL = NNSketch

# Object files
%.o: %.c $(wildcard *.h)
	$(CC) $(CFLAGS) -c -o $@ $<

# Executables
all: $(ALL)

NNSketch: main.o BoundedPriorityQueue.o DynamicTimeWarping.o NearestNeighbours.o Sketch.o easyppm.o

$(ALL):
	$(CC) $(CFLAGS) -o $@ $^

# Phony
.PHONY: all clean run

clean:
	rm -f $(ALL) *.o

run: $(ALL)
	./NNSketch trainingset.txt testset.txt


