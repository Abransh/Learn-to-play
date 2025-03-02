#include "audio/AudioCapture.h"
#include <iostream>

AudioCapture::AudioCapture()
    : stream_(nullptr),
      isInitialized_(false),
      isCapturing_(false),
      sampleRate_(44100),
      framesPerBuffer_(1024) {
}

AudioCapture::~AudioCapture() {
    if (isCapturing_) {
        stop();
    }
    
    if (isInitialized_) {
        Pa_Terminate();
    }
}