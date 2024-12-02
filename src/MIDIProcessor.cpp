#include "MIDIProcessor.h"
#include <iostream>

MIDIProcessor::MIDIProcessor() : midiin(nullptr), noteCallbackFunc(nullptr) {}

MIDIProcessor::~MIDIProcessor() {
    if (midiin) {
        midiin->closePort();
        delete midiin;
    }
    // RtMidi::terminate();
}

bool MIDIProcessor::initialize() {
    try {
        midiin = new RtMidiIn();

        unsigned int nPorts = midiin->getPortCount();
        if (nPorts == 0) {
            std::cout << "No MIDI input ports available.\n";
            delete midiin;
            midiin = nullptr;
            return false;
        }

        // Open the first available port
        midiin->openPort(0);
        std::cout << "Opened MIDI port: " << midiin->getPortName(0) << std::endl;

        // Ignore sysex, timing, and active sensing messages
        midiin->ignoreTypes(true, true, true);

        // Set the callback function
        midiin->setCallback(&MIDIProcessor::staticMidiCallback, this);
    }
    catch (RtMidiError &error) {
        error.printMessage();
        if (midiin) {
            delete midiin;
            midiin = nullptr;
        }
        return false;
    }

    return true;
}

void MIDIProcessor::setNoteCallback(NoteCallback cb) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    noteCallbackFunc = cb;
}

void MIDIProcessor::staticMidiCallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
    MIDIProcessor* handler = static_cast<MIDIProcessor*>(userData);
    if (handler) {
        handler->handleMidiMessage(deltatime, message);
    }
}

void MIDIProcessor::handleMidiMessage(double deltatime, std::vector<unsigned char> *message) {
    if (message->empty()) return;

    unsigned char status = message->at(0) & 0xF0; // Ignore channel
    unsigned char data1 = message->size() > 1 ? message->at(1) : 0;
    unsigned char data2 = message->size() > 2 ? message->at(2) : 0;

    std::lock_guard<std::mutex> lock(callbackMutex);

    if (status == 0x90 && data2 > 0) { // Note On
        if (noteCallbackFunc) {
            noteCallbackFunc(data1, true);
        }
    }
    else if ((status == 0x80) || (status == 0x90 && data2 == 0)) { // Note Off
        if (noteCallbackFunc) {
            noteCallbackFunc(data1, false);
        }
    }
}