
#include <dirent.h>
#include "io.h"
#include "waveform.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int count_rows(char *filename) {
    FILE *filepointer = fopen(filename, "r");
    if (filepointer == NULL) { //Making sure file can be read
        printf("ERROR: Cannot open file.");
        return -1;
    }

    int count = 0;
    char line[256];

    fgets(line, sizeof(line), filepointer);

    while (fgets(line, sizeof(line), filepointer) != NULL) {
        count++; //Count is number of rows. Increases until there are no more rows.
    }

    fclose(filepointer);
    return count;
}

struct WaveformSample *read_log(char *filename, int *rows) {
    *rows = count_rows(filename); // change value of the rows/size variable using pointers

    struct WaveformSample *array = malloc(*rows * sizeof(struct WaveformSample)); //Malloc for memory allocation

    FILE *filepointer = fopen(filename, "r");
    if (filepointer == NULL) { //Making sure file can be read
        printf("ERROR: Cannot open file.");
        return NULL;
    }

    int count = 0;
    char line[256];

    fgets(line, sizeof(line), filepointer);

    if (strcmp(line, "timestamp,phase_A_voltage,phase_B_voltage,phase_C_voltage,line_current,frequency,power_factor,thd_percent\n") != 0) {
        // ^if the first line is not what we expect^
        printf("ERROR: %s does not have the correct headers.\n", filename);
        fclose(filepointer); // close file and free the array early
        free(array);
        return NULL;
    }

    while (fgets(line, sizeof(line), filepointer) != NULL) { //Goes until end of file

        char *token;

        token = strtok(line, ","); //Creating token which is separated by commas
        array[count].timestamp = atof(token); //Placing the current tokens value into the struct

        token = strtok(NULL, ","); //NULL used to continue reading same string
        array[count].phase_A_voltage = atof(token);

        token = strtok(NULL, ",");
        array[count].phase_B_voltage = atof(token);

        token = strtok(NULL, ",");
        array[count].phase_C_voltage = atof(token);

        token = strtok(NULL, ",");
        array[count].line_current = atof(token);

        token = strtok(NULL, ",");
        array[count].frequency = atof(token);

        token = strtok(NULL, ",");
        array[count].power_factor = atof(token);

        token = strtok(NULL, ",");
        array[count].thd_percent = atof(token);

        count++;
    }

    fclose(filepointer);
    return array;
}

void write_results(char *filename, struct WaveformSample *array, int size) {
    // RMS values
    double a_rms = calc_rms(array, size, 'A');
    double b_rms = calc_rms(array, size, 'B');
    double c_rms = calc_rms(array, size, 'C');
    // peak to peak values
    double a_p2p = peak2peak(array, size, 'A');
    double b_p2p = peak2peak(array, size, 'B');
    double c_p2p = peak2peak(array, size, 'C');
    // dc offset
    double a_offset = DC_offset(array, size, 'A');
    double b_offset = DC_offset(array, size, 'B');
    double c_offset = DC_offset(array, size, 'C');
    // compliance with threshold
    int A_compliance = compliance_check(array, size, 'A');
    int B_compliance = compliance_check(array, size, 'B');
    int C_compliance = compliance_check(array, size, 'C');
    // std deviation and variance
    double a_variance = calc_variance(array, size, 'A');
    double b_variance = calc_variance(array, size, 'B');
    double c_variance = calc_variance(array, size, 'C');
    double a_std_dev = calc_std_dev(array, size, 'A');
    double b_std_dev = calc_std_dev(array, size, 'B');
    double c_std_dev = calc_std_dev(array, size, 'C');
    int clip_count = clip_check(array, size);

    FILE *filepointer = fopen(filename, "w");
    if (filepointer == NULL) { //Making sure file can be read
        printf("ERROR: Cannot open file.");
        return;
    }

    fprintf(filepointer, "\nPhase A:\n"
                         "Root-mean-square: %lf\n"
                         "Peak to peak: %lf\n"
                         "DC offset: %lf\n"
                         "Variance: %lf\n"
                         "Standard deviation: %lf\n",
                         a_rms,a_p2p,a_offset, a_variance, a_std_dev);

    // writing whether compliant
    switch (A_compliance) {
        case 1:
            fprintf(filepointer, "Phase A is compliant.\n");
            break;
        case -1:
            fprintf(filepointer, "Phase A is not compliant.\n");
            break;
    }

    fprintf(filepointer, "\nPhase B:\n"
                         "Root-mean-square: %lf\n"
                         "Peak to peak: %lf\n"
                         "DC offset: %lf\n"
                         "Variance: %lf\n"
                         "Standard deviation: %lf\n",
                         b_rms,b_p2p,b_offset, b_variance, b_std_dev);

    switch (B_compliance) {
        case 1:
            fprintf(filepointer, "Phase B is compliant.\n");
            break;
        case -1:
            fprintf(filepointer, "Phase B is not compliant.\n");
            break;
    }

    fprintf(filepointer, "\nPhase C:\n"
                         "Root-mean-square: %lf\n"
                         "Peak to peak: %lf\n"
                         "DC offset: %lf\n"
                         "Variance: %lf\n"
                         "Standard deviation: %lf\n",
                         c_rms,c_p2p,c_offset, c_variance, c_std_dev);

    switch (C_compliance) {
        case 1:
            fprintf(filepointer, "Phase C is compliant.\n");
            break;
        case -1:
            fprintf(filepointer, "Phase C is not compliant.\n");
            break;
    }

    fprintf(filepointer, "\nTimes clipped: %d", clip_count);
    fclose(filepointer);
}

void process_directory(int mode) {
    int rows;
    char path[256];
    int csv_count = 0;
    uint8_t A_status;
    uint8_t B_status;
    uint8_t C_status;

    printf("Please enter the directory path: ");
    scanf(" %255[^\n]", path); // has max character limit of 255 (excluding the null terminator)
    // will end at newline character instead of at space

    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("ERROR: Could not open directory\n");
        return;
    }

    struct dirent *entry; // using struct defined in dirent library for entries in the directory
    while ((entry = readdir(dir)) != NULL) { // ends when checked through all files in directory
        if (strstr(entry->d_name, ".csv") != NULL) { // if there is a substring of .csv in the name of the file:
            csv_count++;
            char filepath[256];
            char results_file[256];

            sprintf(filepath, "%s\\%s", path, entry->d_name); // append the file name onto the end of the directory provided by the user
            sprintf(results_file, "%s\\results%d.txt", path, csv_count); // append/create the results file

            struct WaveformSample *samples = read_log(filepath, &rows);

            switch (mode) {
                case 1:
                    sprintf(results_file, "%s\\results%d.txt", path, csv_count); // append/create the results file

                    if (samples == NULL) { // if there are no samples, for safety
                        printf("ERROR: Could not read %s\n", filepath);
                        continue;
                    }

                    printf("Processing CSV: %s\n", filepath);

                    write_results(results_file, samples, rows);
                    break;
                case 2:
                    if (samples == NULL) {
                        printf("ERROR: Could not read %s\n", filepath);
                        continue;
                    }

                    printf("Sorting CSV: %s\n", filepath);

                    cocktail_sort(samples, rows, filepath);
                    break;
                case 3:
                    if (samples == NULL) {
                        printf("ERROR: Could not read %s\n", filepath);
                        continue;
                    }

                    printf("Checking health of CSV: %s\n", filepath);

                    set_status(samples, rows, &A_status, &B_status, &C_status);
                    write_status(filepath, samples, rows);
                    break;
                case 4:
                    // read/write section
                    sprintf(results_file, "%s\\results%d.txt", path, csv_count); // append/create the results file

                    if (samples == NULL) {
                        printf("ERROR: Could not read %s\n", filepath);
                        continue;
                    }

                    printf("Processing CSV: %s\n", filepath);

                    write_results(results_file, samples, rows);

                    // sorting section
                    printf("Sorting CSV: %s\n", filepath);

                    cocktail_sort(samples, rows, filepath);

                    // health check section
                    printf("Checking health of CSV: %s\n\n", filepath);

                    set_status(samples, rows, &A_status, &B_status, &C_status);
                    write_status(filepath, samples, rows);
                    break;
                default:
                    break;
            }

            free(samples);
        }
    }
    closedir(dir);
}