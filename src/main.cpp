#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>

#include "audio/AudioCapture.h"
#include "Chords/ChordRecognizer.h"

// For Qt GUI (assuming Qt is used)
#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        // Create central widget and layout
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        
        // Create UI elements
        chordLabel = new QLabel("Detected Chord: -", centralWidget);
        chordLabel->setAlignment(Qt::AlignCenter);
        chordLabel->setStyleSheet("font-size: 24pt; font-weight: bold;");
        
        targetChordLabel = new QLabel("Target Chord: C", centralWidget);
        targetChordLabel->setAlignment(Qt::AlignCenter);
        targetChordLabel->setStyleSheet("font-size: 20pt;");
        
        scoreLabel = new QLabel("Score: 0%", centralWidget);
        scoreLabel->setAlignment(Qt::AlignCenter);
        
        QPushButton *startButton = new QPushButton("Start Listening", centralWidget);
        QPushButton *stopButton = new QPushButton("Stop Listening", centralWidget);
        
        // Add widgets to layout
        layout->addWidget(targetChordLabel);
        layout->addWidget(chordLabel);
        layout->addWidget(scoreLabel);
        layout->addWidget(startButton);
        layout->addWidget(stopButton);