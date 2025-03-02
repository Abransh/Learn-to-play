#include "Chords/ChordRecognizer.h"
#include <algorithm>
#include <cmath>
#include <fftw3.h>

ChordRecognizer::ChordRecognizer() {
}

ChordRecognizer::~ChordRecognizer() {
}


void ChordRecognizer::initialize() {
    // Initialize chord templates (relative semitone positions from root)
    chordTemplates_["major"] = {0, 4, 7};       // Major triad (e.g., C-E-G)
    chordTemplates_["minor"] = {0, 3, 7};       // Minor triad (e.g., C-Eb-G)
    chordTemplates_["7th"] = {0, 4, 7, 10};     // Dominant 7th (e.g., C-E-G-Bb)
    chordTemplates_["maj7"] = {0, 4, 7, 11};    // Major 7th (e.g., C-E-G-B)
    chordTemplates_["min7"] = {0, 3, 7, 10};    // Minor 7th (e.g., C-Eb-G-Bb)
    
    // Note names (for all 12 semitones)
    std::vector<std::string> noteNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    
    // Generate the chord library for all roots and all types
    for (int root = 0; root < 12; root++) {
        // For major chords
        {
            Chord chord;
            chord.name = noteNames[root];
            chord.type = "major";
            for (int note : chordTemplates_["major"]) {
                chord.notes.push_back((root + note) % 12);
            }
            chordLibrary_[chord.name] = chord;
        }

        // For minor chords
        {
            Chord chord;
            chord.name = noteNames[root] + "m";
            chord.type = "minor";
            for (int note : chordTemplates_["minor"]) {
                chord.notes.push_back((root + note) % 12);
            }
            chordLibrary_[chord.name] = chord;
        }
        
          // For 7th chords
        {
            Chord chord;
            chord.name = noteNames[root] + "7";
            chord.type = "7th";
            for (int note : chordTemplates_["7th"]) {
                chord.notes.push_back((root + note) % 12);
            }
            chordLibrary_[chord.name + "7"] = chord;
        }

        //any other chord class can be added here, i guess 5th and 9th i should i add but later 

        }
}

std::vector<float> ChordRecognizer::calculateChromagram(const float* buffer, size_t size) {
    // This is a simplified implementation - a real one would use FFT and more sophisticated methods
      std::vector<float> chromagram(12, 0.0f); // 12 semitones in an octave
    
    // Allocate FFTW buffers
     float* in = (float*) fftwf_malloc(sizeof(float) * size);
     fftwf_complex* out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * (size/2 + 1));
    
    // Copy input buffer
    for (size_t i = 0; i < size; i++) {
        in[i] = buffer[i];
    }
    
     // Create FFT plan and execute
    fftwf_plan plan = fftwf_plan_dft_r2c_1d(size, in, out, FFTW_ESTIMATE);
    fftwf_execute(plan);
    
    // Calculate magnitude spectrum
    std::vector<float> magnitudes(size/2 + 1);
    for (size_t i = 0; i < size/2 + 1; i++) {
        float real = out[i][0];
        float imag = out[i][1];
        magnitudes[i] = sqrt(real*real + imag*imag);
    }

    // Map frequencies to pitch classes (very simplified)
    // A proper implementation would use constant-Q transform or a more sophisticated method
    for (size_t i = 1; i < magnitudes.size(); i++) {
        // Convert bin index to frequency
        float frequency = i * 44100.0f / size; // Assuming 44.1kHz sample rate
        
        // Convert frequency to MIDI note number
        float midiNote = 69 + 12 * log2(frequency / 440.0f);
        
        // Convert MIDI note to pitch class (0-11)
        int pitchClass = ((int)round(midiNote) % 12 + 12) % 12;
        
        // Add magnitude to chromagram
        chromagram[pitchClass] += magnitudes[i];
    }

    // Normalize chromagram
    float maxVal = *std::max_element(chromagram.begin(), chromagram.end());
    if (maxVal > 0) {
        for (auto& val : chromagram) {
            val /= maxVal;
        }
    }

    // Clean up FFTW resources
    fftwf_destroy_plan(plan);
    fftwf_free(in);
    fftwf_free(out);
    
    return chromagram;
}

Chord ChordRecognizer::recognizeChord(const std::vector<float>& chromagram) {
    // Calculate correlations with all chord templates
    auto correlations = calculateChordCorrelations(chromagram);
    
    // Find the best matching chord
    std::string bestChord = "";
    float bestCorrelation = -1.0f;
    
    for (const auto& pair : correlations) {
        if (pair.second > bestCorrelation) {
            bestCorrelation = pair.second;
            bestChord = pair.first;
        }
    }
    
    // Return the best matching chord or an "unknown" chord if below threshold
    if (bestCorrelation > 0.6f && !bestChord.empty()) { // Arbitrary threshold
        return chordLibrary_[bestChord];
    } else {
        Chord unknown;
        unknown.name = "Unknown";
        unknown.type = "unknown";
        return unknown;
    }
}


std::map<std::string, float> ChordRecognizer::calculateChordCorrelations(const std::vector<float>& chromagram) {
    std::map<std::string, float> correlations;
    
    for (const auto& pair : chordLibrary_) {
        const Chord& chord = pair.second;
        float correlation = 0.0f;
        
        // Simple correlation - sum the chromagram values for the chord notes
        for (int note : chord.notes) {
            correlation += chromagram[note];
        }
        
        // Normalize by number of notes in the chord
        correlation /= chord.notes.size();
        
        // Store correlation value
        correlations[chord.name] = correlation;
    }

    return correlations;
}

std::vector<int> ChordRecognizer::extractStrongNotes(const std::vector<float>& chromagram, float threshold) {
    std::vector<int> notes;
    
    for (int i = 0; i < 12; i++) {
        if (chromagram[i] > threshold) {
            notes.push_back(i);
        }
    }
    
    return notes;
}

float ChordRecognizer::compareWithTarget(const Chord& detected, const Chord& target) {
    if (detected.name == "Unknown") {
        return 0.0f;
    }
    
    // Count matching notes
    int matches = 0;
    for (int note : detected.notes) {
        if (std::find(target.notes.begin(), target.notes.end(), note) != target.notes.end()) {
            matches++;
        }
    }
    
    // Calculate score (percentage of matching notes)
    float score = (float)matches / std::max(detected.notes.size(), target.notes.size());
    
    return score;
}