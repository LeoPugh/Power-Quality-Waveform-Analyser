

#ifndef POWER_QUALITY_WAVEFORM_ANALYSER_IO_H
#define POWER_QUALITY_WAVEFORM_ANALYSER_IO_H

#include "waveform.h"

int count_rows(char *filename);

struct WaveformSample *read_log(char *filename, int *rows);

#endif //POWER_QUALITY_WAVEFORM_ANALYSER_IO_H