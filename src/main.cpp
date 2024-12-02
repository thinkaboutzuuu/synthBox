// src/main.cpp
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <cmath>
#include <map>
#include <vector>
#include <mutex>
#include "Synth.h"
#include "Oscillator.h"
#include "AudioEngine.h"
#include "MIDIProcessor.h"

// Function to convert MIDI note number to frequency
float midiNoteToFrequency(int note) {
    return 440.0f * pow(2.0f, (note - 69) / 12.0f);
}

int main() {
    try {
        // Create a shared Synth instance
        auto synth = std::make_shared<Synth>(44100);
    
        // Initialize AudioEngine
        AudioEngine audioEngine(synth, 44100, 512);
        if (!audioEngine.start()) {
            std::cerr << "Failed to start audio engine." << std::endl;
            return -1;
        }

        // Initialize MIDIProcessor
        MIDIProcessor mp;
        if (!mp.initialize()) {
            std::cerr << "Failed to initialize MIDI handler." << std::endl;
            // Continue without MIDI
        }

        // Set the MIDI callback
        mp.setNoteCallback([&](int note, bool isOn) {
            float frequency = midiNoteToFrequency(note);
            if (isOn) {
                // Add multiple oscillators for complex waveform
                try {
                    // synth->addOscillator(note, std::make_unique<SineOscillator>(frequency, 44100));
                    // synth->addOscillator(note, std::make_unique<SquareOscillator>(frequency, 44100));
                    synth->addOscillator(note, std::make_unique<SawOscillator>(frequency, 44100));
                    std::cout << "Note On: " << note << " (" << frequency << " Hz)" << std::endl;
                }
                catch (const std::exception& ex) {
                    std::cerr << "Error adding oscillator: " << ex.what() << std::endl;
                }
            }
            else {
                // Remove all oscillators associated with the note
                synth->removeOscillators(note);
                std::cout << "Note Off: " << note << std::endl;
            }
        });

        std::cout << "SynthBox with MIDI is running. Press Ctrl+C to exit." << std::endl;

        // Keep the main thread alive
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return -1;
    }
    
    return 0;
}