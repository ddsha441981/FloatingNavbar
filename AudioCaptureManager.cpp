//
// Created by deendayal on 11/06/25.
//

#include "AudioCaptureManager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>

AudioCaptureManager::AudioCaptureManager(QObject *parent)
    : QObject(parent),
      vadProcessor(new VadProcessor(this))
{
    vadProcessor->initialize();
}

AudioCaptureManager::~AudioCaptureManager() {
    stop();
}

void AudioCaptureManager::setTranscriptCallback(std::function<void(const QString&)> callback) {
    transcriptCallback = callback;
}

void AudioCaptureManager::start() {
    if (!audioWsClient) {
        // Load WebSocket URL from INI file
        QSettings settings("/../configs/wsocket.ini", QSettings::IniFormat);
        QString wsUrl = settings.value("WebSocket/url", "ws://localhost:8080/ws").toString();
        qDebug() << "[AudioCaptureManager] WebSocket URL loaded from config:" << wsUrl;

        // Create WebSocket client
        audioWsClient = new AudioWebSocketClient(QUrl(wsUrl), this);
        connect(audioWsClient, &AudioWebSocketClient::transcriptionReceived,this, &AudioCaptureManager::onTranscriptionReceived);
    }

    if (!captureProcess) {
        captureProcess = new QProcess(this);

        QString audioCapturePath;
#ifdef Q_OS_WIN
        audioCapturePath = QCoreApplication::applicationDirPath() + "/../audio-source/capture_windows.exe";
#else
        audioCapturePath = QCoreApplication::applicationDirPath() + "/../audio-source/SpeakerCapture";
#endif

        qDebug() << "[AudioCaptureManager] Starting process at:" << audioCapturePath;
        QStringList arguments;
        captureProcess->start(audioCapturePath, arguments, QIODevice::ReadOnly);

        if (!captureProcess->waitForStarted()) {
            qWarning() << "[AudioCaptureManager] Failed to start:" << captureProcess->errorString();
        } else {
            connect(captureProcess, &QProcess::readyReadStandardOutput, this, &AudioCaptureManager::onCaptureOutput);
        }
    }
}


void AudioCaptureManager::stop() {
    if (captureProcess) {
        captureProcess->terminate();
        if (!captureProcess->waitForFinished(3000)) {
            captureProcess->kill();
            captureProcess->waitForFinished();
        }
        delete captureProcess;
        captureProcess = nullptr;
        qDebug() << "[AudioCaptureManager] Process stopped";
    }

    if (audioWsClient) {
        audioWsClient->sendCloseMicrophone();
        audioWsClient->deleteLater();
        audioWsClient = nullptr;
        qDebug() << "[AudioCaptureManager] WebSocket closed";
    }

    audioBuffer.clear();
    if (vadProcessor) {
        vadProcessor->resetState();
    }
}

void AudioCaptureManager::onCaptureOutput() {
    QByteArray newData = captureProcess->readAllStandardOutput();
    if (newData.isEmpty()) {
        qDebug() << "[AudioCaptureManager] Empty capture data, skipping";
        return;
    }

    audioBuffer.append(newData);
    sendAudioIfSpeechDetected();
}

void AudioCaptureManager::sendAudioIfSpeechDetected() {
    const int SAMPLE_RATE = 16000;
    const int CHANNELS = 1;
    const int BYTES_PER_SAMPLE = 2;
    const int CHUNK_DURATION_MS = 1000;
    const int chunkSize = SAMPLE_RATE * CHANNELS * BYTES_PER_SAMPLE * (CHUNK_DURATION_MS / 1000);

    static int consecutiveSilence = 0;
    const int maxSilenceChunks = 120;

    while (audioBuffer.size() >= chunkSize) {
        QByteArray chunk = audioBuffer.left(chunkSize);
        if (vadProcessor->processAudio(chunk)) {
            audioWsClient->sendAudioData(chunk);
            consecutiveSilence = 0;
        } else {
            qDebug() << "[AudioCaptureManager] Silence chunk dropped";
            if (++consecutiveSilence >= maxSilenceChunks) {
                stop();
                return;
            }
        }
        audioBuffer.remove(0, chunkSize);
    }
}

void AudioCaptureManager::onTranscriptionReceived(const QString &text) {
    emit transcriptionReceived(text);
    if (transcriptCallback) {
        transcriptCallback(text);
    }
}
