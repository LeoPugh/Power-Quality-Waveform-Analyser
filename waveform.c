

#include "waveform.h"
#include "io.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>

double calc_rms(struct WaveformSample *array, int size) {
    double sum = 0.0;
    double values[size];
    char user_input;

    printf("Would you like RMS for phase A, B or C?\n");
    scanf(" %c", &user_input);

    switch(toupper(user_input))
    {
        case 'A':
            for (int i = 0; i < size; i++) {
                values[i] = array[i].phase_A_voltage;
            }
            break;
        case 'B':
            for (int i = 0; i < size; i++) {
                values[i] = array[i].phase_B_voltage;
            }
            break;
        case 'C':
            for (int i = 0; i < size; i++) {
                values[i] = array[i].phase_C_voltage;
            }
            break;
        default:
            printf("Invalid input\n");
            return -1;
    }

    for (int i = 0; i < size; i++) {
        sum += values[i] * values[i];
    }

    double mean = sum / size;



    return sqrt(mean);
}

double peak2peak(struct WaveformSample *array, int size) {
    double lowest = 0;
    double highest = 0;
    double p2p;

    double values[size];
    for (int i = 0; i < size; i++) {
        values[i] = array[i].phase_A_voltage;
    }
    for (int i = 0; i < size; i++) {
        if (values[i] < lowest) {
            lowest = values[i];
        }
        if (values[i] > highest) {
            highest = values[i];
        }
    }
    p2p = highest - lowest;

    return p2p;
}

void cocktail_sort(struct WaveformSample *array, int size, char *filename) {
    int swapped = 1;
    int start = 0;
    int end = size - 1;

    while (swapped) {
        swapped = 0;

        // forward pass
        for (int i = start; i < end; i++) {
            if (array[i].phase_A_voltage > array[i + 1].phase_A_voltage) {
                struct WaveformSample *a = &array[i];
                struct WaveformSample *b = &array[i + 1];
                struct WaveformSample temp = *a;
                *a = *b; // uses pointers to swap values
                *b = temp;
                swapped = 1;
            }
        }

        if (!swapped) {
            break;
        }

        swapped = 0;
        end--;

        // backward pass
        for (int i = end - 1; i >= start; i--) {
            if (array[i].phase_A_voltage > array[i + 1].phase_A_voltage) {
                struct WaveformSample *a = &array[i];
                struct WaveformSample *b = &array[i + 1];
                struct WaveformSample temp = *a;
                *a = *b;
                *b = temp;
                swapped = 1;
            }
        }

        start++;
    }

    char sorted_file[256];
    sprintf(sorted_file, "%s_sorted.csv", filename); // creating name of new file

    FILE *fp = fopen(sorted_file, "w"); // making/changing the sorted file
    if (fp == NULL) {
        printf("Could not open %s.\n", sorted_file);
        return;
    }

    fprintf(fp, "timestamp,phase_A_voltage,phase_B_voltage,phase_C_voltage,line_current,frequency,power_factor,thd_percent\n");
    // ^write in header^

    for (int i = 0; i < size; i++) { // write in new values after sort
        fprintf(fp, "%f,%f,%f,%f,%f,%f,%f,%f\n",
            array[i].timestamp,
            array[i].phase_A_voltage,
            array[i].phase_B_voltage,
            array[i].phase_C_voltage,
            array[i].line_current,
            array[i].frequency,
            array[i].power_factor,
            array[i].thd_percent);
    }

    fclose(fp);
}
