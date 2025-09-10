// PipeServer.h
#pragma once

#include <QThread>
#include <QString>
#include <windows.h>
#include "PipeWorker.h"


class PipeServer : public QThread {
    Q_OBJECT

public:
    //explicit PipeServer(QObject *parent = nullptr);
explicit PipeServer(const QString &pipeName, QObject *parent = nullptr);

    void stop();

signals:
    void commandReceived(const QString &command);
    void responseReady(const QString &response);  // optional
void commandWithWorker(const QString &command, PipeWorker* worker);

protected:
    void run() override;

private:
	QString pipeName;
    bool running = true;
    HANDLE clientSocket = INVALID_HANDLE_VALUE;  // ✅ Properly declared

};
