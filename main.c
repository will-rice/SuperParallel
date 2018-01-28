//
// Created by William Rice on 1/14/18.
//
#include "SuperParallel.c"
#include <stdio.h>


int main() {

    int size = 1000;
    double scale = 100;
    double randos[size];

    printf("Random Array Size: %i \n", size);
    random_array(randos, size, scale);

    double total = sum(randos, size);
    printf("Sum:  %.2f \n", total);

    double stan = stdev(randos, size);
    printf("Initial Standard Deviation:  %.2f \n", stan);

    for (int i = 0; i < 10; i++) {

        printf("Smoothed Iteration: %i \n", i + 1);
        smooth(randos, size, .01);
        stan = stdev(randos, size);
        printf("Standard Deviation: %.2f \n", stan);

    }
    return 0;
};
