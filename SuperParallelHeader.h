#ifndef SUPERPARALLEL_SUPERPARALLEL_H
#define SUPERPARALLEL_SUPERPARALLEL_H

void* rand_gen(void* args);

void random_array(double array[], int size, double scale);

void* sum_increment(void* args);

double sum(double array[], int size);

void* std_increment(void* args);

double stdev(double array[], int size);

void* smt_increment(void* args);

void smooth(double array[], int size, double w);

void set_threads(int n);

#endif
