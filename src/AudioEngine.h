#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#pragma once
#include <portaudio.h>
#include <memory>
#include "Synth.h"

class AudioEngine {
public:
    AudioEngine(std::shared_ptr<Synth> synthPtr, int sampleRate = 44100, int framesPerBuffer = 512);
    ~AudioEngine();

    // Start audio streaming
    bool start();

    // Stop audio streaming
    bool stop();

private:
    // PortAudio callback
    static int paCallback(const void* inputBuffer, void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void* userData);

    std::shared_ptr<Synth> synth;
    int sampleRate;
    int framesPerBuffer;
    PaStream* stream;
};

#endif
