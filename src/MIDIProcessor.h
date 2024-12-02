// src/MIDIProcessor.h
#ifndef MIDIPROCESSOR_H
#define MIDIPROCESSOR_H

#pragma once

#include "RtMidi.h"
#include <functional>
#include <map>
#include <vector>
#include <mutex>

class MIDIProcessor {
public:
    using NoteCallback = std::function<void(int, bool)>; // Note number, isOn

    MIDIProcessor();
    ~MIDIProcessor();

    // Set the callback function to handle Note On/Off
    void setNoteCallback(NoteCallback cb);

    // Initialize MIDI input
    bool initialize();

private:
    RtMidiIn* midiin;
    NoteCallback noteCallbackFunc;
    std::mutex callbackMutex;

    // Callback function called by RtMidi
    static void staticMidiCallback(double deltatime, std::vector<unsigned char> *message, void *userData);

    // Internal method to handle MIDI messages
    void handleMidiMessage(double deltatime, std::vector<unsigned char> *message);
};


#endif
