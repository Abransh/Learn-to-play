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
