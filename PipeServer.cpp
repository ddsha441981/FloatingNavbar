#include "PipeWorker.h"
#include <QDebug>
#include "PipeServer.h"

//PipeServer::PipeServer(QObject *parent): QThread(parent), running(true) {}
PipeServer::PipeServer(const QString &pipeName, QObject *parent)
    : QThread(parent), pipeName(pipeName), running(true) {}


void PipeServer::run() {
    //const wchar_t* pipeName = L"\\\\.\\pipe\\FloatingNavbarPipe";
std::wstring fullPipeName = L"\\\\.\\pipe\\" + pipeName.toStdWString();
const wchar_t* pipeNameW = fullPipeName.c_str();


    while (running) {
        HANDLE hPipe = CreateNamedPipeW(
            pipeNameW,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            4096, 4096, 0, NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            qDebug() << "[PipeServer] Failed to create pipe.";
            Sleep(1000);
            continue;
        }

        qDebug() << "[PipeServer] Waiting for client connection...";
        BOOL connected = ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED;

        if (connected) {
            qDebug() << "[PipeServer] Client connected!";
            auto *worker = new PipeWorker(hPipe);

            connect(worker, &PipeWorker::commandReady, this, [this](const QString &cmd, PipeWorker *worker) {
                emit commandWithWorker(cmd, worker);
            });

            connect(worker, &QThread::finished, worker, &QObject::deleteLater);

            worker->start();
        } else {
            CloseHandle(hPipe);
        }

        // 🔁 Loop to accept the next connection
    }
}
