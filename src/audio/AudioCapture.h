#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <portaudio.h>
#include <vector>
#include <functional>
#include <memory>

class AudioCapture {
public:
    AudioCapture();
    ~AudioCapture();

    // Initialize audio system
    bool initialize(int sampleRate = 44100, int framesPerBuffer = 1024);
    
    // Start audio capture
    bool start();
    
    // Stop audio capture
    bool stop();
    
    // Check if currently capturing
    bool isCapturing() const;
    
    // Set callback for audio processing
    void setAudioCallback(std::function<void(const float*, size_t)> callback);

private:
    // PortAudio callback function
    static int paCallback(const void* inputBuffer, void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void* userData);

    PaStream* stream_;
    bool isInitialized_;
    bool isCapturing_;
    int sampleRate_;
    int framesPerBuffer_;
    std::function<void(const float*, size_t)> audioCallback_;
};

#endif // AUDIO_CAPTURE_H