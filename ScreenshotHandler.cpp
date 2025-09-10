#include "ScreenshotHandler.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <bits/codecvt.h>

ScreenshotHandler::ScreenshotHandler(QObject *parent): QObject(parent), process(nullptr){
    //changes
    capturingResponse = false;
    readyReceived = false;
    startingProcessor = false;

}

ScreenshotHandler::~ScreenshotHandler()
{
    stopProcessor();
}

bool ScreenshotHandler::startProcessor()
{
    if (isRunning()) {
        qDebug() << "[ScreenshotHandler] Processor already running or starting.";
        return false;
    }
    startingProcessor = true;
    readyReceived = false;
    capturingResponse = false;

    QString processorBinaryDir = QCoreApplication::applicationDirPath() + "/../screen-capture/";
    QString processorBinary = processorBinaryDir + "entrypoint.bin";

    if (!QFile::exists(processorBinary)) {
        qWarning() << "[ScreenshotHandler] Binary not found:" << processorBinary;
        startingProcessor = false;
        return false;
    }

    process = new QProcess(this);
    process->setProgram(processorBinary);
    process->setArguments(QStringList());
    process->setProcessChannelMode(QProcess::MergedChannels);

    connect(process, &QProcess::readyReadStandardOutput, this, &ScreenshotHandler::handleReadyReadStandardOutput);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ScreenshotHandler::handleProcessFinished);
    connect(process, &QProcess::errorOccurred, this, &ScreenshotHandler::handleProcessError);

    process->start();

    if (!process->waitForStarted(5000)) {
        qWarning() << "[ScreenshotHandler] Failed to start processor.";
        delete process;
        process = nullptr;
        startingProcessor = false;
        return false;
    }
    emit newCaptureStarted();  // <--- EMIT THIS SIGNAL

    qDebug() << "[ScreenshotHandler] Processor started.";
    startingProcessor = false;
    return true;
}

void ScreenshotHandler::stopProcessor()
{
    if (process) {
        qDebug() << "[ScreenshotHandler] Stopping processor...";
        process->terminate();
        if (!process->waitForFinished(5000)) {
            qWarning() << "[ScreenshotHandler] Processor did not terminate gracefully. Killing.";
            process->kill();
            process->waitForFinished();
        }
        delete process;
        process = nullptr;
        qDebug() << "[ScreenshotHandler] Processor stopped.";
    }
}

bool ScreenshotHandler::isRunning() const{
    return process && process->state() == QProcess::Running;
}

void ScreenshotHandler::handleReadyReadStandardOutput(){
    while (process->canReadLine()) {
        QByteArray line = process->readLine().trimmed();
        QString lineStr = QString::fromUtf8(line);

        // Detect READY line
        if (lineStr.contains("[READY] Screenshot processor ready.")) {
            qDebug() << "[ScreenshotHandler] Processor READY received.";
            readyReceived = true;
            continue;
        }

        // Main capture logic
        if (lineStr.contains("[LLM] Response received.")) {
            capturingResponse = true;
            continue;
        }

        if (lineStr.startsWith("[") && capturingResponse) {
            capturingResponse = false;
            continue;
        }

        if (capturingResponse && !lineStr.startsWith("[")) {
            emit newProcessorOutput(lineStr);
        }
    }

}



void ScreenshotHandler::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus){
    qWarning() << "[ScreenshotHandler] Processor error:" << std::codecvt_base::error;

    if (process) {
        delete process;
        process = nullptr;
    }

    startingProcessor = false;
    readyReceived = false;
    capturingResponse = false;

    // Optional: Auto-restart
    QTimer::singleShot(2000, this, [this]() {
        qDebug() << "[ScreenshotHandler] Attempting to restart processor...";
        startProcessor();
    });


}

void ScreenshotHandler::handleProcessError(QProcess::ProcessError error){
    qWarning() << "[ScreenshotHandler] Processor error:" << error;
}
