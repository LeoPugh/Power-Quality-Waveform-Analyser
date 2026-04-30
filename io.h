

#ifndef POWER_QUALITY_WAVEFORM_ANALYSER_IO_H
#define POWER_QUALITY_WAVEFORM_ANALYSER_IO_H

#include "waveform.h"

int count_rows(char *filename); //Counts rows so malloc can be used

struct WaveformSample *read_log(char *filename, int *rows); //Mallocs the struct then places values into struct

void write_results(char *filename, struct WaveformSample *array, int size);

void process_directory(int mode);

#endif //POWER_QUALITY_WAVEFORM_ANALYSER_IO_H