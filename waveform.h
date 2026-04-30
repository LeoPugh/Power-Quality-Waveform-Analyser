

#ifndef POWER_QUALITY_WAVEFORM_ANALYSER_WAVEFORM_H
#define POWER_QUALITY_WAVEFORM_ANALYSER_WAVEFORM_H
#include <stdint.h>
#define AC_OK       0b00000001  // bit 0 - 0=fault, 1=healthy
#define UV_FAULT    0b00000010  // bit 1 - under voltage
#define OV_FAULT    0b00000100  // bit 2 - over voltage
#define CLIP_FAULT  0b00001000  // bit 3 - clipping above expected

struct WaveformSample {
    // Creating the fields for each sample
    double timestamp;
    double phase_A_voltage;
    double phase_B_voltage;
    double phase_C_voltage;
    double line_current;
    double frequency;
    double power_factor;
    double thd_percent;
    // Using double as it has a higher length limit and can include decimal values
    uint8_t phase_A_status;
    uint8_t phase_B_status;
    uint8_t phase_C_status;
    // adding the status bitfield
};

double calc_rms(struct WaveformSample *array, int size, char phase);

double peak2peak(struct WaveformSample *array, int size, char phase);

double DC_offset(struct WaveformSample *array, int size, char phase);

int clip_check(struct WaveformSample *array, int size);

int compliance_check(struct WaveformSample *array, int size, char phase);

double calc_variance(struct WaveformSample *array, int size, char phase);

double calc_std_dev(struct WaveformSample *array, int size, char phase);

void cocktail_sort(struct WaveformSample *array, int size, char *filename);

void set_status(struct WaveformSample *array, int size, uint8_t *phase_A_status, uint8_t *phase_B_status, uint8_t *phase_C_status);

void write_status(char *filename, struct WaveformSample *array, int size);

#endif //POWER_QUALITY_WAVEFORM_ANALYSER_WAVEFORM_H