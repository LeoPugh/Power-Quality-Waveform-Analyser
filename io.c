

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