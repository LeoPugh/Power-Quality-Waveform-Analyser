cmake_minimum_required(VERSION 4.1)
project(Power_Quality_Waveform_Analyser C)

set(CMAKE_C_STANDARD 11)

add_executable(Power_Quality_Waveform_Analyser
        main.c
        io.c
        io.h
        waveform.c
        waveform.h
        user_input.c
        user_input.h)

target_link_options(Power_Quality_Waveform_Analyser PRIVATE -mconsole)