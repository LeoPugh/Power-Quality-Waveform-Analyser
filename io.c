

#include "io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int count_rows(char *filename) {
    FILE *filepointer = fopen(filename, "r");
    if (filepointer == NULL) { //Making sure file can be read
        printf("Cannot open file.");
        return 1;
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
    *rows = count_rows(filename);

    struct WaveformSample *array = malloc(*rows * sizeof(struct WaveformSample)); //Malloc for memory allocation

    FILE *filepointer = fopen(filename, "r");
    if (filepointer == NULL) { //Making sure file can be read
        printf("Cannot open file.");
        return NULL;
    }

    int count = 0;
    char line[256];

    fgets(line, sizeof(line), filepointer);

    if (strcmp(line, "timestamp,phase_A_voltage,phase_B_voltage,phase_C_voltage,line_current,frequency,power_factor,thd_percent\n") != 0) {
        // ^if the first line is not what we expect^
        printf("Error: %s does not have the correct headers.\n", filename);
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
    double rms = calc_rms(array, size);
    double p2p = peak2peak(array, size);
    double offset = DC_offset(array, size);
    int clip_count = clip_check(array, size);
    int A_compliance = compliance_check(array, size, 'A');
    int B_compliance = compliance_check(array, size, 'B');
    int C_compliance = compliance_check(array, size, 'C');

    FILE *filepointer = fopen(filename, "w");
    if (filepointer == NULL) { //Making sure file can be read
        printf("Cannot open file.");
        return;
    }
    fprintf(filepointer, "Root-mean-square: %lf\n"
                         "Peak to peak: %lf\n"
                         "DC offset: %lf\n"
                         "Times clipped: %d\n",
                         rms,p2p,offset,clip_count);
    switch (A_compliance) {
        case 1:
            fprintf(filepointer, "Phase A is compliant.\n");
            break;
        case -1:
            fprintf(filepointer, "Phase A is not compliant.\n");
            break;
    }

    switch (B_compliance) {
        case 1:
            fprintf(filepointer, "Phase B is compliant.\n");
            break;
        case -1:
            fprintf(filepointer, "Phase B is not compliant.\n");
            break;
    }

    switch (C_compliance) {
        case 1:
            fprintf(filepointer, "Phase C is compliant.\n");
            break;
        case -1:
            fprintf(filepointer, "Phase C is not compliant.\n");
            break;
    }

    fprintf(filepointer, "Phase A variance: %lf\n", calc_variance(array, size, 'A'));
    fprintf(filepointer, "Phase A standard deviation: %lf\n", calc_std_dev(array, size, 'A'));
    fprintf(filepointer, "Phase B variance: %lf\n", calc_variance(array, size, 'B'));
    fprintf(filepointer, "Phase B standard deviation: %lf\n", calc_std_dev(array, size, 'B'));
    fprintf(filepointer, "Phase C variance: %lf\n", calc_variance(array, size, 'C'));
    fprintf(filepointer, "Phase C standard deviation: %lf\n", calc_std_dev(array, size, 'C'));
    fclose(filepointer);
}

void process_directory() {
    int rows;
    char path[256];
    int csv_count = 0;

    printf("Please enter the directory path: ");
    scanf(" %255[^\n]", path); // has max character limit of 255 (excluding the null terminator)
    // will end at newline character instead of at space

    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("Could not open directory\n");
        return;
    }

    struct dirent *entry; // using struct defined in dirent library for entries in the directory
    while ((entry = readdir(dir)) != NULL) { // ends when checked through all files in directory
        if (strstr(entry->d_name, ".csv") != NULL) { // if there is a substring of .csv in the name of the file:
            csv_count++;
            char filepath[512];
            char results_file[512];

            sprintf(filepath, "%s\\%s", path, entry->d_name); // append the file name onto the end of the directory provided by the user
            sprintf(results_file, "%s\\results%d.txt", path, csv_count); // append/create the results file

            printf("Processing CSV: %s\n", filepath);

            struct WaveformSample *samples = read_log(filepath, &rows);
            if (samples == NULL) { // if there are no samples, for safety
                printf("Could not read %s\n", filepath);
                continue;
            }

            write_results(results_file, samples, rows);
            free(samples);
        }
    }
    closedir(dir);
}