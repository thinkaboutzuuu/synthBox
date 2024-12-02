// src/Synth.cpp
#include "Synth.h"
#include <cstring> // For memset

Synth::Synth(int sampleRate)
    : sr(sampleRate) {}

void Synth::addOscillator(int note, std::unique_ptr<Oscillator> osc) {
    std::lock_guard<std::mutex> lock(synthMutex);
    if (activeNotes.find(note) == activeNotes.end()) {
        activeNotes[note] = OscillatorGroup();
    }
    activeNotes[note].oscillators.emplace_back(std::move(osc));
}

void Synth::removeOscillators(int note) {
    std::lock_guard<std::mutex> lock(synthMutex);
    auto it = activeNotes.find(note);
    if (it != activeNotes.end()) {
        activeNotes.erase(it);
    }
}

void Synth::generateAudio(float* buffer, int frameCount) {
    // Initialize buffer to zero
    memset(buffer, 0, sizeof(float) * frameCount);

    std::lock_guard<std::mutex> lock(synthMutex);

    // Sum outputs from all oscillators across all active notes
    for (auto& pair : activeNotes) {
        for (auto& osc : pair.second.oscillators) {
            osc->generate(buffer, frameCount);
        }
    }

    // Optional: Apply normalization or limiting here to prevent clipping
    for (int i = 0; i < frameCount; ++i) {
        if (buffer[i] > 1.0f) buffer[i] = 1.0f;
        if (buffer[i] < -1.0f) buffer[i] = -1.0f;
    }
}