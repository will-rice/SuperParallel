#include "SuperParallelHeader.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

void random_array(double array[], int size, double scale) {

    srand(time(0));
    for (int i = 0; i < size; i++) {
        array[i] = rand() % (int) scale / scale;
    }
}


double sum(double array[], int size){

    double sum = 0.0;
    for (int i = 0; i < size; i++){
        sum += array[i];
    }

    return sum;
}


double stdev(double array[], int size){

    double stdev = 0.0;
    double sum = 0.0;
    double mean = 0.0;
    double variance = 0.0;
    for (int i = 0; i < size; i++){
        sum += array[i];
    }
    mean = sum / size;
    for (int i = 0; i < size; i++){
        variance += (array[i] - mean) * (array[i] - mean);
    }
    variance = variance / size;
    stdev = sqrt(variance);

    return stdev;
}


void smooth(double array[], int size, double w){

    for (int i = 0; i < size - 1; i++){
        array[i + 1] = array[i] * w + (array[i - 1] + array[i + 1]) * (1 - w) / 2;
    }
}
