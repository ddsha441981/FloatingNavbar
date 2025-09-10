#ifndef SCREENSHOTHANDLER_H
#define SCREENSHOTHANDLER_H

#include <QObject>
#include <QProcess>

class ScreenshotHandler : public QObject
{
    Q_OBJECT
public:
    explicit ScreenshotHandler(QObject *parent = nullptr);
    ~ScreenshotHandler();

    bool startProcessor();  // Start watching (if not already running)
    void stopProcessor();   // Stop watching
    bool isRunning() const; // Check if watcher is running

    signals:
        void newProcessorOutput(const QString &output);
        void newCaptureStarted();

private slots:
    void handleReadyReadStandardOutput();
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleProcessError(QProcess::ProcessError error);

private:
    QProcess *process;
    //changes
    bool capturingResponse = false;      // Moved from static to member
    bool readyReceived = false;          // New: to detect READY signal
    bool startingProcessor = false;      // New: to prevent double start

};

#endif // SCREENSHOTHANDLER_H
