#ifndef AUDIOCAPTUREMANAGER_H
#define AUDIOCAPTUREMANAGER_H

#include <QObject>
#include <QProcess>
#include <QByteArray>
#include "audiowsclient.h"
#include "VadProcessor.h"

class AudioCaptureManager : public QObject {
    Q_OBJECT

public:
    explicit AudioCaptureManager(QObject *parent = nullptr);
    ~AudioCaptureManager();

    void start();
    void stop();

    void setTranscriptCallback(std::function<void(const QString&)> callback);

    signals:
        void transcriptionReceived(const QString &text);

private slots:
    void onCaptureOutput();
    void onTranscriptionReceived(const QString &text);

private:
    void sendAudioIfSpeechDetected();

    QProcess *captureProcess = nullptr;
    AudioWebSocketClient *audioWsClient = nullptr;
    VadProcessor *vadProcessor = nullptr;

    QByteArray audioBuffer;
    std::function<void(const QString&)> transcriptCallback;
};

#endif // AUDIOCAPTUREMANAGER_H
