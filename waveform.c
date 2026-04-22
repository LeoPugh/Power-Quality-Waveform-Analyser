

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