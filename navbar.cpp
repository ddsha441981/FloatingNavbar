/***************************************************************
 * Date       : 26 May 2025
 * Developed By : Deendayal Kumawat
 ***************************************************************/


#include "navbar.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QScreen>
#include <QPixmap>
#include <QGuiApplication>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QDebug>
#include <QShortcut>
#include <QKeySequence>
#include <QSysInfo>
#include <QOperatingSystemVersion>

#include <QTimer>
#include <QAudioFormat>
// #include <QAudioDeviceInfo>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "moreoptionsdialog.h"
#include "PromptLoader.h"
#include "ScreenshotHandler.h"
#include "ConfigUtil.h"
#include "SettingsDialog.h"
#include <QSettings>
#include "PipeServer.h"
#include "PipeWorker.h"
#include <cstring>  // For memcpy
#include <vector>

#include "ModelSelectionDialog.h"
#include <QProcess>

static void safeShow(QWidget* w) {
    if (!w || w->isVisible())
        return;

    w->show();
    w->raise();
}

static void safeHide(QWidget* w) {
    if (!w || !w->isVisible())
        return;

    w->hide();
}

static void safeToggleVisible(QWidget* w) {
    if (!w) return;
    if (w->isVisible()) {
        safeHide(w);
    } else {
        safeShow(w);
    }
}


//Constructor
Navbar::Navbar(QWidget *parent) : QWidget(parent) {

    //PipeServer *pipeServer = new PipeServer;
	//pipeServer = new PipeServer;
    //pipeServer->start();



	// Pipe Server
	//QObject::connect(pipeServer, &PipeServer::commandReceived, this, &Navbar::handlePipeCommand);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setFixedHeight(50);
    setStyleSheet("background-color: black;");

    // 🧱 Outer layout (centering container)
    auto *outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // 🎯 Inner layout (holds buttons)
    auto *layout = new QHBoxLayout();
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(10);

    // 🎛️ Buttons
    QPushButton *btnPrompt = new QPushButton("Prompt", this);
    QPushButton *btnCapture = new QPushButton("Capture", this);
    QPushButton *btnSend    = new QPushButton("Ask/Help", this);
    QPushButton *btnToggleAudio = new QPushButton("Start Listening", this);
    QPushButton *btnShowHide  = new QPushButton("Show/Hide", this);
    QPushButton *btnTranscript = new QPushButton("Transcript", this);
    QPushButton *btnMore = new QPushButton("More", this);

    QString buttonStyle = R"(
        QPushButton {
            background-color: #444;
            color: white;
            border: none;
            padding: 4px 8px;
            border-radius: 4px;
            font-weight: normal;
            min-height: 10px;
            min-width: 20px;
        }
        QPushButton:hover {
            background-color: #666;
        }
        QPushButton:pressed {
            background-color: #222;
        }
    )";

    // ✨ Style & SizePolicy
    QList<QPushButton*> buttons = {btnPrompt, btnCapture, btnSend, btnToggleAudio, btnShowHide, btnTranscript, btnMore};
    for (auto *btn : buttons) {
        btn->setStyleSheet(buttonStyle);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        layout->addWidget(btn);
    }

    // ✅ Center button row
    outerLayout->addStretch();
    outerLayout->addLayout(layout);
    outerLayout->addStretch();

    adjustSize();
    int screenWidth = QGuiApplication::primaryScreen()->geometry().width();
    move((screenWidth - width()) / 2, 0);

    // ✅ Init Groq
    groqStreamer = new GroqStreamer(this);
    connect(groqStreamer, &GroqStreamer::tokenReceived, this, [this](const QString &token){
        currentStreamingText += token;
        if (messagePopup) {
		//If we need to show response of AI in FloatingApp uncomment it
            messagePopup->showMessage(currentStreamingText, this);
        }
 	// ✅ Live stream to Win32 overlay pipe
     if (pendingAskHelpWorker) {
        pendingAskHelpWorker->sendStreamingData(token);
     }
    });

//send response Win32 app
connect(groqStreamer, &GroqStreamer::streamingFinished, this, [this]() {
    if (!currentStreamingText.isEmpty()) {
        if (pendingAskHelpWorker) {
			pendingAskHelpWorker->sendStreamingData("\n[Streaming finished]\n");
            pendingAskHelpWorker->sendResponse(currentStreamingText);
            pendingAskHelpWorker = nullptr;
        } else if (pendingAskHelpWorker) {
            pendingAskHelpWorker->sendResponse(currentStreamingText); // fallback (not needed if using workers only)
        }
    }
    qDebug() << "Echomind streaming finished.";
    currentStreamingText.clear();
    bufferedTranscript.clear();
});

    connect(groqStreamer, &GroqStreamer::streamingFailed, this, [this](const QString &error){
        qDebug() << "Echomind streaming failed:" << error;
        if (messagePopup) {
            messagePopup->showMessage("Error: " + error, this);
        }
        bufferedTranscript.clear();
    });

    networkManager = new QNetworkAccessManager(this);
    screenshotHandler = new ScreenshotHandler(this);
    screenshotResponseView = new ScreenshotResponseWindow();
    transcriptionView = new TranscriptWindow();
    messagePopup = new MessagePopup(this);

    connect(screenshotHandler, &ScreenshotHandler::newProcessorOutput, this, [this](const QString &output) {
        if (!screenshotResponseView->isVisible()) {
            screenshotResponseView->showLeftOf(this);
        }
        screenshotResponseView->append(output);
    });

    connect(screenshotHandler, &ScreenshotHandler::newCaptureStarted, this, [this]() {
        screenshotResponseView->clear();
        if (!screenshotResponseView->isVisible()) {
            screenshotResponseView->showLeftOf(this);
        }
    });

    // 🧠 Button logic
    connect(btnCapture, &QPushButton::clicked, this, &Navbar::captureScreen);

    connect(btnSend, &QPushButton::clicked, this, [this]() {
        sendRestApi(bufferedTranscript);
    });

    connect(btnToggleAudio, &QPushButton::clicked, this, [this, btnToggleAudio]() {
        if (!isListening) {
            launchAudioCapture();
            btnToggleAudio->setText("Stop Listening");
            isListening = true;
        } else {
            stopAudioCapture();
            btnToggleAudio->setText("Start Listening");
            isListening = false;
        }
    });

    connect(btnShowHide, &QPushButton::clicked, this, [this]() {
        if (this->isVisible()) {
            safeHide(this);
            safeHide(transcriptionView);
            safeHide(screenshotResponseView);
            safeHide(messagePopup);
            safeHide(promptWindow);
        } else {
            safeShow(this);
        }
    });

    connect(btnTranscript, &QPushButton::clicked, this, &Navbar::toggleTranscriptView);
    connect(btnPrompt, &QPushButton::clicked, this, &Navbar::togglePromptWindow);

    connect(btnMore, &QPushButton::clicked, this, [this, btnMore]() {
        if (!moreDialog) {
            moreDialog = new MoreOptionsDialog(this);

            connect(moreDialog, &MoreOptionsDialog::settingsClicked, this, [this]() {
                qDebug() << "Settings clicked.";
                SettingsDialog settingsDialog(this);
                QPoint globalPos = moreDialog->mapToGlobal(QPoint(0, moreDialog->height()));
                settingsDialog.move(globalPos.x(), globalPos.y());
                settingsDialog.exec();
            });

            connect(moreDialog, &MoreOptionsDialog::contextShortcutClicked, this, [this]() {
                qDebug() << "Select Model clicked.";
                ModelSelectionDialog modelDialog(this);
                QPoint dialogPos = moreDialog->mapToGlobal(QPoint(0, moreDialog->height()));
                modelDialog.move(dialogPos.x() + 10, dialogPos.y());
                if (modelDialog.exec() == QDialog::Accepted) {
                    QSettings settings;
                    settings.setValue("selectedModel", modelDialog.getSelectedModelId());
                }
            });

            connect(moreDialog, &MoreOptionsDialog::resetEveyThing, this, [this]() {
                qDebug() << "Reset everything";
                resetFloatingEverything();
            });
        }

        QPoint globalPos = btnMore->mapToGlobal(QPoint(0, btnMore->height()));
        moreDialog->move(globalPos.x() + 10, globalPos.y());
        moreDialog->exec();
    });

    // 🌐 Global Hotkey: Ctrl+Alt+Shift+N
    globalHotkey = new QHotkey(QKeySequence("Ctrl+Alt+Shift+N"), true, this);
    connect(globalHotkey, &QHotkey::activated, [this]() {
        qDebug() << "Global hotkey pressed — showing Navbar";
        safeShow(this);
    });

    // Final initializations
    vadProcessor = new VadProcessor(this);
    vadProcessor->initialize();
}


void Navbar::setPipeServers(PipeServer* cmd, PipeServer* transcript, PipeServer* help, PipeServer* capture) {
    connect(cmd, &PipeServer::commandWithWorker, this, [this](const QString &cmd, PipeWorker* worker) {
        qDebug() << "🔘 CMD Pipe: Received command:" << cmd;
        handleGeneralCommand(cmd, worker);
    });

    connect(transcript, &PipeServer::commandWithWorker, this, [this](const QString &cmd, PipeWorker* worker) {
        qDebug() << "🎙️ Transcript Pipe: Received command:" << cmd;
        if (cmd == "Transcript") {
            toggleTranscriptView();
            transcriptStreamingWorker = worker;
            worker->sendStreamingData("[Transcript stream initialized]\n");
        }
    });

    connect(help, &PipeServer::commandWithWorker, this, [this](const QString &cmd, PipeWorker* worker) {
        qDebug() << "🤖 Help Pipe: Received command:" << cmd;
        if (cmd == "Ask/Help") {
            pendingAskHelpWorker = worker;
            sendRestApi(bufferedTranscript);  // will stream
        }
    });

    connect(capture, &PipeServer::commandWithWorker, this, [this](const QString &cmd, PipeWorker* worker) {
        qDebug() << "📸 Capture Pipe: Received command:" << cmd;
    	if (cmd == "Capture") {
        	handleCaptureCommand(worker);  //  Much cleaner
    	}
    });
}

void Navbar::handleGeneralCommand(const QString &cmd, PipeWorker* worker) {
    if (cmd == "Ask/Help") {
        pendingAskHelpWorker = worker;
        sendRestApi(bufferedTranscript);

    } else if (cmd == "Prompt") {
        togglePromptWindow();
        worker->sendResponse("STATUS::Prompt processing.");

    } else if (cmd == "Transcript") {
        qDebug() << "✅ Transcript command received from overlay";
        toggleTranscriptView();
        transcriptStreamingWorker = worker;
        worker->sendStreamingData("[Transcript stream initialized]\n");

    } else if (cmd == "Capture") {
    	qDebug() << "📸 Capture Pipe: Received command.";
    	captureStreamingWorker = worker;
    	screenshotResponseView->clear();         // Optional UI reset
    	captureScreen();      // Start streaming

    	// Optionally notify overlay
    	worker->sendStreamingData("[Capture stream initialized]\n");

    } else if (cmd == "Show/Hide") {
        if (this->isVisible()) {
            safeHide(this);
            safeHide(transcriptionView);
            safeHide(screenshotResponseView);
            safeHide(messagePopup);
            safeHide(promptWindow);
            worker->sendResponse("STATUS::Navbar hidden.");
        } else {
            safeShow(this);
            worker->sendResponse("STATUS::Navbar shown.");
        }

    } else if (cmd == "Start/Stop") {
        QPushButton* toggleBtn = findChild<QPushButton*>("Start Listening");
        if (!isListening) {
            launchAudioCapture();
            if (toggleBtn) toggleBtn->setText("Stop Listening");
            isListening = true;
            worker->sendResponse("STATUS::Listening started.");
        } else {
            stopAudioCapture();
            if (toggleBtn) toggleBtn->setText("Start Listening");
            isListening = false;
            worker->sendResponse("STATUS::Listening stopped.");
        }

    } else {
        worker->sendResponse("Unknown command: " + cmd);
    }
}


//caputure command handle
void Navbar::handleCaptureCommand(PipeWorker* worker) {
    captureStreamingWorker = worker;

    // Clear UI and start processor (safe for both UI + backend)
    captureScreen();

    // Stream start message
    if (worker) {
        worker->sendStreamingData("[Capture stream initialized]\n");
    }
}



// Launch screen watcher tool
void Navbar::captureScreen(){

    if (screenshotHandler->isRunning()) {
        qDebug() << "[Navbar] Screenshot processor already running.";
    } else {
        qDebug() << "[Navbar] Starting screenshot processor.";
        screenshotHandler->startProcessor();
    }

    // 💥 Clear previous responses:
    screenshotResponseView->clear();

    // 💥 Show window again (even if already visible):
    if (!screenshotResponseView->isVisible()) {
        screenshotResponseView->showLeftOf(this);
        QCoreApplication::processEvents();  // Force UI update
    }
}

//handle restapi call
void Navbar::sendRestApi(const QString &transcript) {
    if (transcript.trimmed().isEmpty()) {
        qDebug() << "No transcript to send.";
        return;
    }
    QString apiKey = readApiKeyFromCustomPath();

    if (apiKey.isEmpty()) {
        qWarning() << "API key is empty or couldn't be read!";
        return;
    }

    // 🧠 Extract questions from the transcript
    QList<ClassifiedQuestion> questions = QuestionExtractor::extractQuestions(transcript);
    for (const auto &q : questions) {
        qDebug() << " - Question:" << q.question << "| Type:" << q.type;
    }

    //Clear currentStreamingText and popup before starting new stream
    currentStreamingText.clear();
    if (messagePopup) {
        // popup resets for when new message received
        messagePopup->clearAndHide();
    }


    //After Question QuestionExtracted
    QStringList questionList;
    for (const auto &q : questions) {
        questionList << q.question;
    }
    QString extractedQuestionText = questionList.join("\n");

    if (!extractedQuestionText.trimmed().isEmpty()) {
        qDebug() << "questions extracted.";
        groqStreamer->startStreaming(extractedQuestionText, apiKey);
    } else {
        qDebug() << "No questions extracted. Using full transcript.";
        //Without QuestionExtracted
        groqStreamer->startStreaming(transcript, apiKey);
    }
}

//call external cpp and capture system audio
void Navbar::launchAudioCapture() {
    if (!audioCaptureManager) {
        audioCaptureManager = new AudioCaptureManager(this);

        connect(audioCaptureManager, &AudioCaptureManager::transcriptionReceived,
                this, &Navbar::onTranscriptionReceived);

        // Optional: if you want to do something with the transcript text directly
        audioCaptureManager->setTranscriptCallback([this](const QString &text) {
            qDebug() << "[Navbar] Callback text:" << text;
        });
    }

    transcriptionView->move(this->x() + this->width(), this->y());
    audioCaptureManager->start();
}

//Stop system audio to capture
void Navbar::stopAudioCapture() {
    if (audioCaptureManager) {
        audioCaptureManager->stop();
    }
}

//when transcript receive
void Navbar::onTranscriptionReceived(const QString &rawText) {
    if (!transcriptionView)
        return;

    QString text = rawText.trimmed();
    if (text.startsWith("\"") && text.endsWith("\""))
        text = text.mid(1, text.length() - 2);
    text = text.trimmed();

    if (text.isEmpty())
        return;

    if (bufferedTranscript.endsWith(text + " ")) {
        qDebug() << "Redundant line skipped:" << text;
        return;
    }

    transcriptionView->append(text);
    bufferedTranscript += text + " ";

    // Send to Win32 overlay (streaming)
    if (transcriptStreamingWorker) {
        qDebug() << "✅ Sending to pipe:" << text;
        transcriptStreamingWorker->sendStreamingData(text + "\n");
    }
    //TODO: if you need show transcript when start button then uncomment it
    // safeShow(transcriptionView);
    qDebug() << "Transcription added:" << text;
}

//Transcript
void Navbar::toggleTranscriptView() {
    if (!transcriptionView)
        return;

    if (transcriptionView->isVisible()) {
        safeHide(transcriptionView);
    } else {
        transcriptionView->showRightOf(this);
    }
}

// prompt
void Navbar::togglePromptWindow() {
    if (!promptWindow) {
        promptWindow = new PromptWindow();
    }

    if (!isPromptSent) {
        QString promptText = PromptLoader::readPromptText();
        if (promptText.isEmpty()) {
            qWarning() << "Prompt text is empty. Aborting.";
            return;
        }

        bufferedTranscript = promptText;
        //Send Transcript to AI
        sendRestApi(bufferedTranscript);

        isPromptSent = true;

        QPushButton *btn = qobject_cast<QPushButton *>(sender());
        if (btn) {
            btn->setEnabled(false);
        }
        return;
    }

    if (promptWindow->isVisible()) {
        safeHide(promptWindow);
    } else {
        int x = this->x() - promptWindow->width();
        int y = this->y();
        promptWindow->move(x, y);
        safeShow(promptWindow);
    }
}

void Navbar::resetFloatingEverything() {
    qDebug() << "Resetting everything...";

    // Stop audio capture
    stopAudioCapture();

    // Clear transcript and state
    bufferedTranscript.clear();
    if (transcriptionView) {
        transcriptionView->clear();
    }

    // Clear prompt state
    isPromptSent = false;

    // Clear message popup
    if (messagePopup) {
        messagePopup->clearAndHide();
    }

    // Stop and clear screenshot capture
    if (screenshotHandler && screenshotHandler->isRunning()) {
        screenshotHandler->stopProcessor();
    }

    // Hide screenshot response window
    if (screenshotResponseView && screenshotResponseView->isVisible()) {
        screenshotResponseView->hide();
        qDebug() << "Screenshot response window hidden.";
    }
    qDebug() << "Reset complete.";
}
