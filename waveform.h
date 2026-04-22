

#ifndef POWER_QUALITY_WAVEFORM_ANALYSER_WAVEFORM_H
#define POWER_QUALITY_WAVEFORM_ANALYSER_WAVEFORM_H

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
};

double calc_rms(struct WaveformSample *array, int size);

double peak2peak(struct WaveformSample *array, int size);



#endif //POWER_QUALITY_WAVEFORM_ANALYSER_WAVEFORM_H