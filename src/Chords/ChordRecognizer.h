#ifndef CHORD_RECOGNIZER_H
#define CHORD_RECOGNIZER_H

#include <string>
#include <vector>
#include <map>
#include <Eigen/Dense>

// Struct to represent a chord
struct Chord {
    std::string name;       // Chord name (e.g., "C", "Am")
    std::string type;       // Chord type (e.g., "major", "minor", "7th")
    std::vector<int> notes; // MIDI note numbers for the chord
};

class ChordRecognizer {
public:
    ChordRecognizer();
    ~ChordRecognizer();

    // Initialize with common chords
    void initialize();
    
    // Process audio buffer and detect chord
    Chord recognizeChord(const std::vector<float>& chromagram);
    
    // Calculate chromagram from audio buffer
    std::vector<float> calculateChromagram(const float* buffer, size_t size);
    
    // Compare detected notes with the expected chord (for scoring)
    float compareWithTarget(const Chord& detected, const Chord& target);

    private:
    // Map of chord names to chord definitions
    std::map<std::string, Chord> chordLibrary_;
    
    // Templates for common chord types
    std::map<std::string, std::vector<int>> chordTemplates_;
    
    // Calculate the correlation between detected notes and chord templates
    std::map<std::string, float> calculateChordCorrelations(const std::vector<float>& chromagram);
    
    // Utility function to extract the strongest notes from chromagram
    std::vector<int> extractStrongNotes(const std::vector<float>& chromagram, float threshold = 0.5f);
};

#endif // CHORD_RECOGNIZER_H