#ifndef VADPROCESSOR_H
#define VADPROCESSOR_H

#include <QObject>
#include <QByteArray>
#include <fvad.h>

class VadProcessor : public QObject {
    Q_OBJECT
public:
    explicit VadProcessor(QObject *parent = nullptr);
    ~VadProcessor();

    bool initialize(int sampleRate = 16000, int frameLengthMs = 10, int mode = 2);
    bool processAudio(const QByteArray& audioData);
    void resetState();

    bool isSpeechDetected() const { return speechDetected; }

private:
    Fvad* vadInst = nullptr;
    bool vadInitialized = false;
    bool speechDetected = false;

    int vadSampleRate = 16000;
    int vadFrameLength = 10;
    int vadMode = 2;
};

#endif // VADPROCESSOR_H
