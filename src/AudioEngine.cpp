// src/AudioEngine.cpp
#include "AudioEngine.h"
#include <iostream>
#include <cstring> // For memset

AudioEngine::AudioEngine(std::shared_ptr<Synth> synthPtr, int sampleRate, int framesPerBuffer)
    : synth(synthPtr), sampleRate(sampleRate), framesPerBuffer(framesPerBuffer), stream(nullptr) {
    Pa_Initialize();
}

AudioEngine::~AudioEngine() {
    stop();
    Pa_Terminate();
}

bool AudioEngine::start() {
    PaError err;

    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if (outputParameters.device == paNoDevice) {
        std::cerr << "Error: No default output device." << std::endl;
        return false;
    }

    outputParameters.channelCount = 1;       // Mono output
    outputParameters.sampleFormat = paFloat32; // 32-bit floating point
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(
        &stream,
        nullptr, // No input
        &outputParameters,
        sampleRate,
        framesPerBuffer,
        paClipOff, // We won't output out of range samples
        &AudioEngine::paCallback,
        this // Pass 'this' as user data
    );

    if (err != paNoError) {
        std::cerr << "Error opening PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Error starting PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    std::cout << "Audio stream started." << std::endl;
    return true;
}

bool AudioEngine::stop() {
    if (stream) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = nullptr;
        std::cout << "Audio stream stopped." << std::endl;
    }
    return true;
}

int AudioEngine::paCallback(const void* inputBuffer, void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData) {
    AudioEngine* engine = static_cast<AudioEngine*>(userData);
    float* out = static_cast<float*>(outputBuffer);

    // Generate audio from Synth
    engine->synth->generateAudio(out, framesPerBuffer);

    // Since we're using paClipOff, ensure no samples are out of range [-1.0, 1.0]
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        if (out[i] > 1.0f) out[i] = 1.0f;
        if (out[i] < -1.0f) out[i] = -1.0f;
    }

    return paContinue;
}