/***************************************************************
* Date       : 26 May 2025
 * Developed By : Deendayal Kumawat
 ***************************************************************/


#ifndef NAVBAR_H
#define NAVBAR_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QAudioInput>
#include <QIODevice>
#include <QTimer>
#include "audiowsclient.h"
#include <QTextEdit>
#include <QSet>

#include "GroqStreamer.h"
#include "MessagePopup.h"
#include "moreoptionsdialog.h"
#include "TranscriptWindow.h"
#include "PromptWindow.h"
#include "ScreenshotHandler.h"
#include "ScreenshotResponseWindow.h"
#include "qhotkey.h"
#include <fvad.h>
#include "VadProcessor.h"
#include "QuestionExtractor.h"
#include <QScopedPointer>
#include "AudioCaptureManager.h"
#include "pipeserver.h"
#include "PipeWorker.h"



class Navbar : public QWidget {
    Q_OBJECT
public:
    explicit Navbar(QWidget *parent = nullptr);
    // ~Navbar();  // Add destructor to clean up VAD
    bool isListening = false;
void setPipeServers(PipeServer* cmd, PipeServer* transcript, PipeServer* help, PipeServer* capture);
void handleGeneralCommand(const QString &cmd, PipeWorker* worker);


private slots:
    void captureScreen();
    void launchAudioCapture();
    void stopAudioCapture();
    void onTranscriptionReceived(const QString &text);
    void toggleTranscriptView();
    void togglePromptWindow();
    void resetFloatingEverything();
    //void sendRestApi(const QString &transcript);
    void sendRestApi(const QString &transcript);
void handleCaptureCommand(PipeWorker* worker);


private:
    QNetworkAccessManager *networkManager;
    QString lastTranscript;
    QSet<QString> seenTranscripts;
    MessagePopup *messagePopup = nullptr;
    TranscriptWindow *transcriptionView = nullptr;
    QString bufferedTranscript;
    PromptWindow *promptWindow = nullptr;
    MoreOptionsDialog *moreDialog = nullptr;
    bool isPromptSent = false;
    GroqStreamer *groqStreamer;
    QString currentStreamingText;
    ScreenshotHandler *screenshotHandler;
    ScreenshotResponseWindow *screenshotResponseView;
    QHotkey* globalHotkey = nullptr;
    QProcess *captureProcess = nullptr;
    QuestionExtractor questionExtractor;
    AudioCaptureManager *audioCaptureManager = nullptr;
    ScreenshotResponseWindow *screenshotWindow = nullptr;
	PipeServer* pipeServer = nullptr;
PipeWorker* pendingHelpWorker = nullptr;
PipeWorker* pendingAskHelpWorker = nullptr;
PipeWorker* transcriptStreamingWorker = nullptr;
PipeWorker* captureStreamingWorker = nullptr;






    // Audio capture & websocket
    QAudioInput *audioInput = nullptr;
    QIODevice *audioDevice = nullptr;
    AudioWebSocketClient *audioWsClient = nullptr;
    bool isPaused = false;

    QByteArray audioBuffer;
    QTimer *sendTimer = nullptr;
    VadProcessor *vadProcessor = nullptr;
};

#endif // NAVBAR_H