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

          // Set central widget
        setCentralWidget(centralWidget);
        
        // Initialize audio and chord recognition
        audioCapture = std::make_unique<AudioCapture>();
        audioCapture->initialize();
        
        chordRecognizer = std::make_unique<ChordRecognizer>();
        chordRecognizer->initialize();
        
        // Set audio callback
        audioCapture->setAudioCallback([this](const float* buffer, size_t size) {
            processAudio(buffer, size);
        });

        // Connect signals and slots
        connect(startButton, &QPushButton::clicked, this, &MainWindow::startListening);
        connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopListening);
        
        // Setup update timer (update UI 10 times per second)
        updateTimer = new QTimer(this);
        connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateUI);
        updateTimer->start(100);
        
        // Window settings
        setWindowTitle("Guitar Learning App");
        resize(600, 400);
    }

    ~MainWindow() {
        if (audioCapture && audioCapture->isCapturing()) {
            audioCapture->stop();
        }
    }

    private slots:
    void startListening() {
        if (audioCapture) {
            audioCapture->start();
        }
    }
    
    void stopListening() {
        if (audioCapture) {
            audioCapture->stop();
        }
    }


    void updateUI() {
        // Update UI with the latest detected chord and score
        chordLabel->setText("Detected Chord: " + QString::fromStdString(lastDetectedChord.name));
        
        // Color the label based on match quality
        if (lastDetectedChord.name == targetChord.name) {
            chordLabel->setStyleSheet("font-size: 24pt; font-weight: bold; color: green;");
        } else {
            chordLabel->setStyleSheet("font-size: 24pt; font-weight: bold; color: red;");
        }
        
        scoreLabel->setText(QString("Score: %1%").arg(int(lastScore * 100)));
    }