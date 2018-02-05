#include "SuperParallelHeader.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

static int NUM_THREADS = 0;
struct thread_args
{
  int rank;
  int size;
  double* array;
  double scale;
  double weight;
  double sum;
  double mean;
  double stdev;
  double variance;
};

void* rand_gen(void* args)
{
  srand(42);
  struct thread_args local;
  local.array[local.rank] = (rand() % (int) local.scale) / local.scale;
}


void random_array(double array[], int size, double scale)
{
  pthread_t thread_ids[NUM_THREADS];
  struct thread_args master[NUM_THREADS];

  for (int t=0; t < NUM_THREADS; t++)
  {
    master[t].size = size;
    master[t].scale = scale;
    master[t].rank = t;
    master[t].array[t] = array[t];
    pthread_create(&thread_ids[t], NULL, rand_gen, &master[t]);
  }
  for (int i=0; i<NUM_THREADS; i++)
  {
    pthread_join(thread_ids[i], NULL);
  }
}


void* sum_increment(void* args)
{
  struct thread_args local;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&lock);
  local.sum += local.array[local.rank];
  pthread_mutex_unlock(&lock);
}


double sum(double array[], int size)
{
  struct thread_args args;

  pthread_t threads[NUM_THREADS];

    for (int i=0; i<NUM_THREADS; i++)
    {
      args.array[i] = array[i];
      pthread_create(&threads[i], NULL, sum_increment, NULL);
    }

    for (int i=0; i<NUM_THREADS; i++)
    {
      pthread_join(threads[i], NULL);
    }

    return args.sum;
}

void* std_increment(void* args)
{
  struct thread_args local;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&lock);
  local.variance += (
    (local.array[local.rank] - local.mean) *
    (local.array[local.rank] - local.mean)) /
     local.size;

  local.stdev += sqrt(local.variance);
  pthread_mutex_unlock(&lock);
}

/*TODO: Look at stdev formula to verify*/
double stdev(double array[], int size){

  struct thread_args master;
  master.mean = master.sum / master.size;
  pthread_t threads[NUM_THREADS];

    for (int i=0; i<NUM_THREADS; i++)
    {
      pthread_create(&threads[i], NULL, std_increment, NULL);
    }

    for (int i=0; i<NUM_THREADS; i++)
    {
      pthread_join(threads[i], NULL);
    }

    return master.stdev;
}

void* smt_increment(void* args)
{
  struct thread_args local;
  double unsmooth = local.array[local.rank];

  double smooth = local.array[local.rank + 1] =
    local.array[local.rank] * local.weight +
    (local.array[local.rank - 1] +
     local.array[local.rank + 1]) *
     (1 - local.weight) / 2;

  local.array[local.rank] = smooth;

}
/* @TODO parallelize*/
void smooth(double array[], int size, double w)
{

  struct thread_args master;
  master.weight = w;
  pthread_t threads[NUM_THREADS];

    for (int i=0; i<NUM_THREADS; i++)
    {
      master.array[i] = array[i];
      pthread_create(&threads[i], NULL, std_increment, NULL);
    }

    for (int i=0; i<NUM_THREADS; i++)
    {
      pthread_join(threads[i], NULL);
    }

    return;
}

void set_threads(int n)
{
  NUM_THREADS = n;
}
