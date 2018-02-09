//
// Created by William Rice on 1/14/18.
//
#include "SuperParallel.c"
#include <stdio.h>
#include <sys/time.h>
struct timeval start, end;

int main()
{

    int size = 0;
    double scale = 100;
    double randos[size];
    set_threads(4);

    gettimeofday(&start, NULL);
    random_array(randos, size, scale);
    gettimeofday(&end, NULL);
    double seconds = (end.tv_usec - start.tv_usec);
    printf("Random Array Size: %i Time Elapsed: %f seconds \n",
           size, seconds);

    gettimeofday(&start, NULL);
    double total = sum(randos, size);
    gettimeofday(&end, NULL);
    seconds = (end.tv_usec - start.tv_usec);
    printf("Global Sum:  %.2f Time Elapsed: %f seconds \n",
           total, seconds);

    gettimeofday(&start, NULL);
    double stan = stdev(randos, size);
    gettimeofday(&end, NULL);
    seconds = (end.tv_usec - start.tv_usec);
    printf(
      "Initial Standard Deviation:  %f Time Elapsed: %f seconds \n",
       stan, seconds);

    gettimeofday(&start, NULL);
    smooth(randos, size, .01);
    stan = stdev(randos, size);
    gettimeofday(&end, NULL);
    seconds = (end.tv_usec - start.tv_usec);
    printf("Standard Deviation: %f Time Elapsed: %f seconds \n ",
           stan, seconds);

    return 0;
}
