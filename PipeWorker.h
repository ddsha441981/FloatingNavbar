//
// Created by deendayal kumawat on 6/16/2025.
//

// PipeWorker.h
// PipeWorker.h
#pragma once

#include <QThread>
#include <windows.h>
#include <QString>

class PipeWorker : public QThread {
    Q_OBJECT
public:
    explicit PipeWorker(HANDLE hPipe, QObject *parent = nullptr);
    void run() override;

    signals:
        void commandReady(const QString &command, PipeWorker *worker);

public slots:
    void sendResponse(const QString &text);
void sendStreamingData(const QString &text);


private:
    HANDLE hPipe;
};


