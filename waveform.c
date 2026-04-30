#include "waveform.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

double calc_rms(struct WaveformSample *array, int size, char phase) {
    double sum = 0.0;

    switch (phase) {
        case 'A':
            for (int i = 0; i < size; i++) {
                sum += array[i].phase_A_voltage * array[i].phase_A_voltage;
            }
            break;
        case 'B':
            for (int i = 0; i < size; i++) {
                sum += array[i].phase_B_voltage * array[i].phase_B_voltage;
            }
            break;
        case 'C':
            for (int i = 0; i < size; i++) {
                sum += array[i].phase_C_voltage * array[i].phase_C_voltage;
            }
            break;
    }


    double mean = sum / size;
    return sqrt(mean);
}

double peak2peak(struct WaveformSample *array, int size, char phase) {
    double lowest = 0.0;
    double highest = 0.0;
    double p2p;

    switch (phase) {
        case 'A':
            for (int i = 0; i < size; i++) {
                if (array[i].phase_A_voltage < lowest) {
                    lowest = array[i].phase_A_voltage;
                }
                if (array[i].phase_A_voltage > highest) {
                    highest = array[i].phase_A_voltage;
                }
            }
            break;
        case 'B':
            for (int i = 0; i < size; i++) {
                if (array[i].phase_B_voltage < lowest) {
                    lowest = array[i].phase_B_voltage;
                }
                if (array[i].phase_B_voltage > highest) {
                    highest = array[i].phase_B_voltage;
                }
            }
            break;
        case 'C':
            for (int i = 0; i < size; i++) {
                if (array[i].phase_C_voltage < lowest) {
                    lowest = array[i].phase_C_voltage;
                }
                if (array[i].phase_C_voltage > highest) {
                    highest = array[i].phase_C_voltage;
                }
            }
            break;
    }

    p2p = highest - lowest;

    return p2p;
}

double DC_offset(struct WaveformSample *array, int size, char phase) {
    double lowest = 0.0;
    double highest = 0.0;
    double offset;

    switch (phase) {
        case 'A':
            for (int i = 0; i < size; i++) {
                if (array[i].phase_A_voltage < lowest) {
                    lowest = array[i].phase_A_voltage;
                }
                if (array[i].phase_A_voltage > highest) {
                    highest = array[i].phase_A_voltage;
                }
            }
            break;
        case 'B':
            for (int i = 0; i < size; i++) {
                if (array[i].phase_B_voltage < lowest) {
                    lowest = array[i].phase_B_voltage;
                }
                if (array[i].phase_B_voltage > highest) {
                    highest = array[i].phase_B_voltage;
                }
            }
            break;
        case 'C':
            for (int i = 0; i < size; i++) {
                if (array[i].phase_C_voltage < lowest) {
                    lowest = array[i].phase_C_voltage;
                }
                if (array[i].phase_C_voltage > highest) {
                    highest = array[i].phase_C_voltage;
                }
            }
            break;
    }

    offset = highest + lowest;

    return offset;
}

int clip_check(struct WaveformSample *array, int size) {
    double threshold = 324.9;
    int clip_count = 0.0;

    for (int i = 0; i < size; i++) {
        if (array[i].phase_A_voltage <= -threshold || array[i].phase_A_voltage >= threshold) {
            clip_count++;
        }
    }

    return clip_count;
}

int compliance_check(struct WaveformSample *array, int size, char phase) {
    double sum = 0.0;
    double desired_value = 230.0;

    switch(phase) {
        case 'A':
            for (int i = 0; i < size; i++) {
                sum += array[i].phase_A_voltage * array[i].phase_A_voltage;
            }
            break;
        case 'B':
            for (int i = 0; i < size; i++) {
                sum += array[i].phase_B_voltage * array[i].phase_B_voltage;
            }
            break;
        case 'C':
            for (int i = 0; i < size; i++) {
                sum += array[i].phase_C_voltage * array[i].phase_C_voltage;
            }
            break;
    }
    double mean = sum / size;

    if (sqrt(mean) < (desired_value*0.9) || sqrt(mean) > (desired_value*1.1)) { // if not compliant
        return-1;
    }

    return 1; // If compliant
}

double calc_variance(struct WaveformSample *array, int size, char phase) {
    double mean;
    double sum = 0.0;

    for (int i = 0; i < size; i++) {
        switch(phase) {
            case 'A':
                sum += array[i].phase_A_voltage;
                break;
            case 'B':
                sum += array[i].phase_B_voltage;
                break;
            case 'C':
                sum += array[i].phase_C_voltage;
                break;
        }
    }
    mean = sum / size;

    sum = 0.0;
    for (int i = 0; i < size; i++) {
        double diff;
        switch(phase) {
            case 'A':
                diff = array[i].phase_A_voltage - mean;
                break;
            case 'B':
                diff = array[i].phase_B_voltage - mean;
                break;
            case 'C':
                diff = array[i].phase_C_voltage - mean;
                break;
        }
        sum += diff * diff;
    }
    return sum / size;
}

double calc_std_dev(struct WaveformSample *array, int size, char phase) {
    return sqrt(calc_variance(array, size, phase));
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
    char temp[256];

    // removing the .csv at the end of the file in order to add it on manually with _sorted
    strcpy(temp, filename);
    char *csv_check = strstr(temp, ".csv");  // check if file name has .csv at the end
    if (csv_check != NULL) {
        *csv_check = '\0';  // place exit character instead of csv
    }

    sprintf(sorted_file, "%s_sorted.csv", temp); // creating name of new file

    FILE *fp = fopen(sorted_file, "w"); // making/changing the sorted file
    if (fp == NULL) {
        printf("ERROR: Could not open %s.\n", sorted_file);
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

void set_status(struct WaveformSample *array, int size, uint8_t *phase_A_status, uint8_t *phase_B_status, uint8_t *phase_C_status) {
    *phase_A_status = 0;
    *phase_B_status = 0;
    *phase_C_status = 0;
    double min_voltage = 207.0;
    double max_voltage = 253.0;
    double threshold = 325.0;
    int expected_clips = 20;
    int clip_A = 0, clip_B = 0, clip_C = 0;

    double rms_A = calc_rms(array, size, 'A');
    double rms_B = calc_rms(array, size, 'B');
    double rms_C = calc_rms(array, size, 'C');

    for (int i = 0; i < size; i++) {
        if (array[i].phase_A_voltage > threshold || array[i].phase_A_voltage < -threshold) {
            clip_A++;
        }
        if (array[i].phase_B_voltage > threshold || array[i].phase_B_voltage < -threshold) {
            clip_B++;
        }
        if (array[i].phase_C_voltage > threshold || array[i].phase_C_voltage < -threshold) {
            clip_C++;
        }
    }

    // phase A
    if (rms_A < min_voltage) {
        *phase_A_status |= UV_FAULT;
    }
    if (rms_A > max_voltage) {
        *phase_A_status |= OV_FAULT;
    }
    if (clip_A > expected_clips) {
        *phase_A_status |= CLIP_FAULT;
    }
    if (*phase_A_status == 0) {
        *phase_A_status |= AC_OK;
    }

    // phase B
    if (rms_B < min_voltage) {
        *phase_B_status |= UV_FAULT;
    }
    if (rms_B > max_voltage) {
        *phase_B_status |= OV_FAULT;
    }
    if (clip_B > expected_clips) {
        *phase_B_status |= CLIP_FAULT;
    }
    if (*phase_B_status == 0) {
        *phase_B_status |= AC_OK;
    }

    // phase C
    if (rms_C < min_voltage) {
        *phase_C_status |= UV_FAULT;
    }
    if (rms_C > max_voltage) {
        *phase_C_status |= OV_FAULT;
    }
    if (clip_C > expected_clips) {
        *phase_C_status |= CLIP_FAULT;
    }
    if (*phase_C_status == 0) {
        *phase_C_status |= AC_OK;
    }
}

void write_status(char *filename, struct WaveformSample *array, int size) {
    uint8_t A_status, B_status, C_status;

    set_status(array, size, &A_status, &B_status, &C_status);

    char status_file[256];
    char temp[256];

    strcpy(temp, filename);
    char *csv_check = strstr(temp, ".csv");
    if (csv_check != NULL) {
        *csv_check = '\0';
    }
    sprintf(status_file, "%s_status.txt", temp);

    FILE *fp = fopen(status_file, "w");
    if (fp == NULL) {
        printf("ERROR: Could not open status file.\n");
        return;
    }

    // phase A
    if (A_status & AC_OK) {
        fprintf(fp, "Phase A: Healthy\n");
    } else {
        fprintf(fp, "Phase A: Fault\n");
    }
    if (A_status & UV_FAULT)   fprintf(fp, "  Phase A RMS below under voltage threshold\n");
    if (A_status & OV_FAULT)   fprintf(fp, "  Phase A RMS above over voltage threshold\n");
    if (A_status & CLIP_FAULT) fprintf(fp, "  Phase A clipping above expected threshold\n");

    // phase B
    if (B_status & AC_OK) {
        fprintf(fp, "Phase B: Healthy\n");
    } else {
        fprintf(fp, "Phase B: Fault\n");
    }
    if (B_status & UV_FAULT)   fprintf(fp, "  Phase B RMS below under voltage threshold\n");
    if (B_status & OV_FAULT)   fprintf(fp, "  Phase B RMS above over voltage threshold\n");
    if (B_status & CLIP_FAULT) fprintf(fp, "  Phase B clipping above expected threshold\n");

    // phase C
    if (C_status & AC_OK) {
        fprintf(fp, "Phase C: Healthy\n");
    } else {
        fprintf(fp, "Phase C: Fault\n");
    }
    if (C_status & UV_FAULT)   fprintf(fp, "  Phase C RMS below under voltage threshold\n");
    if (C_status & OV_FAULT)   fprintf(fp, "  Phase C RMS above over voltage threshold\n");
    if (C_status & CLIP_FAULT) fprintf(fp, "  Phase C clipping above expected threshold\n");

    fclose(fp);
}