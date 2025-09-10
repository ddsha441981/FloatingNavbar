//
// Created by deendayal kumawat on 6/16/2025.
//

// PipeWorker.cpp
#include "PipeWorker.h"
#include <QDebug>
#include <QByteArray>

PipeWorker::PipeWorker(HANDLE pipe, QObject *parent)
    : QThread(parent), hPipe(pipe) {}

void PipeWorker::run() {
    char buffer[1024] = {0};
    DWORD bytesRead;
    if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
        //QString cmd = QString::fromUtf8(buffer, bytesRead);
        QString cmd = QString::fromUtf8(buffer, bytesRead).trimmed();
        emit commandReady(cmd, this);
    } else {
        CloseHandle(hPipe);
        return;
    }
    exec();  // keep thread alive until sendResponse is called
}

void PipeWorker::sendResponse(const QString &text) {
    if (hPipe == INVALID_HANDLE_VALUE) return;

    QByteArray data = text.toUtf8();
    DWORD written;
    WriteFile(hPipe, data.constData(), data.size(), &written, NULL);
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    hPipe = INVALID_HANDLE_VALUE;
    quit();
}

void PipeWorker::sendStreamingData(const QString &text) {
    if (hPipe == INVALID_HANDLE_VALUE){
		qDebug() << "❌ Pipe is closed!";
		return;
	}

 qDebug() << "✅ Pipe write:" << text;


    QByteArray data = text.toUtf8();
    DWORD written;
    WriteFile(hPipe, data.constData(), data.size(), &written, NULL);
    FlushFileBuffers(hPipe);
    // Do NOT disconnect/close pipe here
}

