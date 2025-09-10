extern "C" {
#include <fvad.h>
}

#include "VadProcessor.h"
#include <QDebug>

VadProcessor::VadProcessor(QObject *parent) : QObject(parent) {}

VadProcessor::~VadProcessor() {
    if (vadInst) {
        fvad_free(vadInst);
    }
}

bool VadProcessor::initialize(int sampleRate, int frameLengthMs, int mode) {
    vadInst = fvad_new();
    if (!vadInst) {
        qWarning() << "Failed to create VAD instance";
        return false;
    }

    vadSampleRate = sampleRate;
    vadFrameLength = frameLengthMs;
    vadMode = mode;

    if (fvad_set_mode(vadInst, vadMode) != 0) {
        qWarning() << "Failed to set VAD mode";
        fvad_free(vadInst);
        vadInst = nullptr;
        return false;
    }

    if (fvad_set_sample_rate(vadInst, vadSampleRate) != 0) {
        qWarning() << "Failed to set VAD sample rate";
        fvad_free(vadInst);
        vadInst = nullptr;
        return false;
    }

    vadInitialized = true;
    speechDetected = false;
    qDebug() << "VAD initialized successfully";
    return true;
}

bool VadProcessor::processAudio(const QByteArray& audioData) {
    if (!vadInitialized || !vadInst) {
        return true;  // fallback: send all audio
    }

    if (audioData.isEmpty()) {
        qWarning() << "Empty audio data passed to VAD";
        return false;
    }

    const int16_t* samples = reinterpret_cast<const int16_t*>(audioData.constData());
    size_t numSamples = audioData.size() / sizeof(int16_t);

    size_t samplesPerFrame = (vadSampleRate * vadFrameLength) / 1000;
    size_t numFrames = numSamples / samplesPerFrame;

    if (numFrames == 0) {
        return speechDetected;  // not enough data yet
    }

    bool currentSpeechDetected = false;

    for (size_t i = 0; i < numFrames; ++i) {
        const int16_t* frame = samples + (i * samplesPerFrame);
        int vadResult = fvad_process(vadInst, frame, samplesPerFrame);

        if (vadResult < 0) {
            qWarning() << "VAD processing error";
            continue;
        }

        if (vadResult == 1) {
            currentSpeechDetected = true;
        }
    }

    speechDetected = currentSpeechDetected;
    return speechDetected;
}

void VadProcessor::resetState() {
    speechDetected = false;
}
