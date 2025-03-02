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

bool AudioCapture::initialize(int sampleRate, int framesPerBuffer) {
    sampleRate_ = sampleRate;
    framesPerBuffer_ = framesPerBuffer;
    
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    
    isInitialized_ = true;
    return true;
}

bool AudioCapture::start() {
    if (!isInitialized_) {
        std::cerr << "Audio system not initialized" << std::endl;
        return false;
    }
    
    if (isCapturing_) {
        return true; // Already capturing
    }
    
    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
        std::cerr << "No default input device" << std::endl;
        return false;
    }
    inputParameters.channelCount = 1; // Mono
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;
    
    PaError err = Pa_OpenStream(&stream_,
                               &inputParameters,
                               nullptr, // No output
                               sampleRate_,
                               framesPerBuffer_,
                               paClipOff,
                               &AudioCapture::paCallback,
                               this);
    
    if (err != paNoError) {
        std::cerr << "Failed to open stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    err = Pa_StartStream(stream_);
    if (err != paNoError) {
        std::cerr << "Failed to start stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream_);
        stream_ = nullptr;
        return false;
    }
    
    isCapturing_ = true;
    return true;
}

bool AudioCapture::stop() {
    if (!isCapturing_) {
        return true; // Not capturing
    }
    
    PaError err = Pa_StopStream(stream_);
    if (err != paNoError) {
        std::cerr << "Failed to stop stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    
    err = Pa_CloseStream(stream_);
    if (err != paNoError) {
        std::cerr << "Failed to close stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    
    stream_ = nullptr;
    isCapturing_ = false;
    return true;
}

bool AudioCapture::isCapturing() const {
    return isCapturing_;
}

void AudioCapture::setAudioCallback(std::function<void(const float*, size_t)> callback) {
    audioCallback_ = callback;
}

int AudioCapture::paCallback(const void* inputBuffer, void* outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void* userData) {
    // Cast user data to AudioCapture instance
    AudioCapture* capture = static_cast<AudioCapture*>(userData);
    
    // Call the user-defined callback if available
    if (capture->audioCallback_) {
        const float* input = static_cast<const float*>(inputBuffer);
        capture->audioCallback_(input, framesPerBuffer);
    }
    
    return paContinue;
}