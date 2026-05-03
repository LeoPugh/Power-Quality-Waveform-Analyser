

#include "user_input.h"
#include <stdio.h>
#include <stdlib.h>
#include "io.h"

void ask_user() {

    char user_input;
    int rows;
    struct WaveformSample *samples = read_log("power_quality_log.csv", &rows);
    uint8_t A_status;
    uint8_t B_status;
    uint8_t C_status;

    do { // using do while to run once before checking
        printf("-------------------------------------\n");
        printf("What would you like to do? (1-9)\n\n"
               "1: Read/write results for set file.\n"
               "2: Read/write results for directory.\n"
               "3: Sort set file.\n"
               "4: Sort directory.\n"
               "5: Check health of set file.\n"
               "6: Check health of directory.\n"
               "7: Do all for set file.\n"
               "8: Do all for directory.\n"
               "9: Exit program.\n");
        scanf(" %c", &user_input);
        printf("-------------------------------------\n");

        switch (user_input) {
            case '1':
                write_results("results.txt", samples, rows);
                break;
            case '2':
                process_directory(1);
                break;
            case '3':
                cocktail_sort(samples, rows, "power_quality_log.csv");
                break;
            case '4':
                process_directory(2);
                break;
            case '5':
                set_status(samples, rows, &A_status, &B_status, &C_status);
                write_status("power_quality_log.csv", samples, rows);
                break;
            case '6':
                process_directory(3);
                break;
            case '7':
                write_results("results.txt", samples, rows);
                cocktail_sort(samples, rows, "power_quality_log.csv");
                set_status(samples, rows, &A_status, &B_status, &C_status);
                write_status("power_quality_log.csv", samples, rows);
                break;
            case '8':
                process_directory(4);
                break;
            case '9':
                break;
            default:
                printf("Invalid input, try again.\n");
        }
    } while (user_input != '9');

    free(samples);
}
