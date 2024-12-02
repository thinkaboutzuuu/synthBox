// src/Synth.h
#ifndef SYNTH_H
#define SYNTH_H

#pragma once
#include <vector>
#include <memory>
#include <map>
#include <mutex>
#include "Oscillator.h"

struct OscillatorGroup {
    std::vector<std::unique_ptr<Oscillator>> oscillators;
};

class Synth {
public:
    Synth(int sampleRate = 44100);
    ~Synth() = default;

    void addOscillator(int note, std::unique_ptr<Oscillator> osc);

    void removeAllOscillators();

    void removeOscillators(int note);


    // Generate audio by summing oscillator outputs
    // The buffer should be pre-allocated with 'frameCount' samples
    void generateAudio(float* buffer, int frameCount);

private:
    int sr;
    std::map<int, OscillatorGroup> activeNotes;
    std::mutex synthMutex; 
};

#endif
