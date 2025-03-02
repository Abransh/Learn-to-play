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