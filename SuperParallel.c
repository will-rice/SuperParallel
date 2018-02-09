#include "SuperParallelHeader.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

static int NUM_THREADS;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


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

int min(int x, int y)
{
  if (x < y){return x;}
  else{return y;}
}
void* rand_gen(void* args)
{
  struct thread_args* local = (struct thread_args*) args;
  double* array = local->array;
  int size = local->size;
  int rank = local->rank;
  double scale = local->scale;
  int index_count = size / NUM_THREADS;
  int first_index = rank * index_count + min(rank, size % NUM_THREADS);
  if (rank < size % NUM_THREADS)
  {
    index_count++;
  }

  for (int i=first_index; i < first_index + index_count; i++)
  {
    array[i] = rand() % (int) scale / scale;
  }
  return NULL;
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
    master[t].array = array;
    pthread_create(&thread_ids[t], NULL, rand_gen, &master[t]);
  }
  for (int i=0; i<NUM_THREADS; i++)
  {
    pthread_join(thread_ids[i], NULL);
  }
  return;
}


void* sum_increment(void* args)
{
  struct thread_args* local = (struct thread_args*) args;
  double* array = local->array;
  int rank = local->rank;
  int size = local->size;
  int index_count = size / NUM_THREADS;
  int first_index = rank * index_count + min(rank, size % NUM_THREADS);
  if (rank < size % NUM_THREADS)
  {
    index_count++;
  }

  for (int i=first_index; i < first_index + index_count; i++)
  {
    pthread_mutex_lock(&lock);
    local->sum += array[i];
    pthread_mutex_unlock(&lock);
  }

  pthread_exit(args);
}


double sum(double array[], int size)
{
  struct thread_args master[NUM_THREADS];
  pthread_t threads[NUM_THREADS];
  double sum = 0.0;
    for (int i=0; i<NUM_THREADS; i++)
    {

      master[i].array = array;
      master[i].sum = sum;
      master[i].rank = i;
      master[i].size = size;
      pthread_create(&threads[i], NULL, sum_increment, &master[i]);
    }

    for (int i=0; i<NUM_THREADS; i++)
    {
      pthread_join(threads[i], NULL);
      sum += master[i].sum;
    }

    return sum;
}

void* std_increment(void* args)
{
  struct thread_args* local = (struct thread_args*) args;
  double* array = local->array;
  int size = local->size;
  int rank = local->rank;
  double mean = local->mean;
  int index_count = size / NUM_THREADS;
  int first_index = rank * index_count + min(rank, size % NUM_THREADS);
  if (rank < size % NUM_THREADS)
  {
    index_count++;
  }

  for (int i=first_index; i < first_index + index_count; i++)
  {
    pthread_mutex_lock(&lock);
    local->variance += (array[i] - mean) * (array[i] - mean);
    pthread_mutex_unlock(&lock);
  }
  pthread_exit(args);
}

/*TODO: Look at stdev formula to verify*/
double stdev(double array[], int size){

  struct thread_args master[NUM_THREADS];
  pthread_t threads[NUM_THREADS];
  double stdev = 0.0;
  double mean = sum(array, size) / size;
  double variance = 0.0;
    for (int i=0; i<NUM_THREADS; i++)
    {
      master[i].array = array;
      master[i].mean = mean;
      master[i].size = size;
      master[i].rank = i;
      master[i].variance = variance;
      pthread_create(&threads[i], NULL, std_increment, &master[i]);
    }

    for (int i=0; i<NUM_THREADS; i++)
    {
      pthread_join(threads[i], NULL);
      stdev += master[i].variance;
    }
    stdev = stdev / size;
    stdev = sqrt(stdev);
    return stdev;
}

void* smt_increment(void* args)
{
  struct thread_args* local = (struct thread_args*) args;
  double* array = local->array;
  int size = local->size;
  double weight = local->weight;
  int rank = local->rank;

  int index_count = size / NUM_THREADS;
  int first_index = rank * index_count + min(rank, size % NUM_THREADS);
  if (rank < size % NUM_THREADS)
  {
    index_count++;
  }

  for (int i=first_index; i<first_index + index_count; i++)
  {
    pthread_mutex_lock(&lock);
    array[i + 1] = (array[i] *
                   weight) +
                   ((array[i - 1] +
                    array[i + 1]) *
                   (1 - weight) / 2);
    pthread_mutex_unlock(&lock);

  }
  pthread_exit(args);

}

void smooth(double array[], int size, double w)
{

  struct thread_args master[NUM_THREADS];
  pthread_t threads[NUM_THREADS];

    for (int i=0; i<NUM_THREADS; i++)
    {
      master[i].array = array;
      master[i].size = size;
      master[i].weight = w;
      master[i].rank = i;

      pthread_create(&threads[i], NULL, std_increment, &master[i]);
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
