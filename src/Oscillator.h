// src/Oscillator.h
#ifndef OSCILLATOR_H
#define OSCILLATOR_H


#pragma once
#include <cmath>

class Oscillator {
public:
    Oscillator(float freq, int rate)
        : freq(freq), sr(rate), phase(0.0f) {}
    virtual ~Oscillator() = default;

    // Generate 'frameCount' samples and fill the buffer
    virtual void generate(float* buffer, int frameCount) = 0;

protected:
    float freq;    // freq in Hz
    int sr;     // Sample rate in Hz
    float phase;        // Current phase
};


class SineOscillator : public Oscillator {
public:
    SineOscillator(float freq, int rate)
        : Oscillator(freq, rate) {}
    virtual ~SineOscillator() override = default;

    virtual void generate(float* buffer, int frameCount) override{
        float phaseIncrement = 2.0f * static_cast<float>(M_PI) * freq / sr;
        for (int i = 0; i < frameCount; ++i) {
            buffer[i] += sinf(phase); // Accumulate to allow multiple oscillators
            phase += phaseIncrement;
            if (phase >= 2.0f * static_cast<float>(M_PI)) {
                phase -= 2.0f * static_cast<float>(M_PI);
            }
        }
    }
};

class SquareOscillator : public Oscillator {
public:
    SquareOscillator(float freq, int rate)
        : Oscillator(freq, rate) {}
    virtual ~SquareOscillator() override = default;

    virtual void generate(float* buffer, int frameCount) override {
    float phaseIncrement = 2.0f * static_cast<float>(M_PI) * freq / sr;
    for (int i = 0; i < frameCount; ++i) {
        buffer[i] += (sinf(phase) >= 0.0f) ? 1.0f : -1.0f; // Square wave
        phase += phaseIncrement;
        if (phase >= 2.0f * static_cast<float>(M_PI)) {
            phase -= 2.0f * static_cast<float>(M_PI);
        }
    }
}
};

class SawOscillator : public Oscillator {
public:
    SawOscillator(float freq, int rate)
        : Oscillator(freq, rate) {}
    virtual ~SawOscillator() override = default;

    virtual void generate(float* buffer, int frameCount) override {
    float phaseIncrement = freq / sr; // Normalized phase increment
    for (int i = 0; i < frameCount; ++i) {
        buffer[i] += 2.0f * (phase - floorf(phase + 0.5f)); // Sawtooth wave
        phase += phaseIncrement;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
    }
}
};



#endif
