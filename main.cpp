/***************************************************************
* Date       : 26 May 2025
 * Developed By : Deendayal Kumawat
 ***************************************************************/

#include <QApplication>
#include "navbar.h"
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <windows.h>


QProcess* goServerProcess = nullptr;
PipeServer* pipeServerCmd = nullptr;
PipeServer* pipeServerTranscript = nullptr;
PipeServer* pipeServerHelp = nullptr;
PipeServer* pipeServerCapture = nullptr;


void startGoServer() {
    // Base path is one level up from your executable's directory, then "audioserver"
    QString basePath = QCoreApplication::applicationDirPath() + "/../audioserver";

    QString program;

#ifdef Q_OS_WIN
    program = basePath + "/go-server-echomind.exe";
#elif defined(Q_OS_MACOS)
    program = basePath + "/go-server-echomind-mac";
#else
    program = basePath + "/go-server-echomind-linux";
#endif

    // Normalize to absolute path (resolve ".." and symlinks)
    program = QFileInfo(program).absoluteFilePath();

    qDebug() << "Trying to start Go server at:" << program;

    goServerProcess = new QProcess();

#ifdef Q_OS_WIN
    // Hide terminal window on Windows
    goServerProcess->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args) {
        args->flags |= CREATE_NO_WINDOW;
    });
#endif

    // Start the server process (using the recommended overload)
    goServerProcess->start(program, QStringList());

    if (!goServerProcess->waitForStarted()) {
        qWarning() << "Failed to start Go server:" << goServerProcess->errorString();
    } else {
        qDebug() << "Go server started.";
    }
}

void stopGoServer() {
    if (goServerProcess) {
        goServerProcess->terminate();
        if (!goServerProcess->waitForFinished(3000)) {
            // If it doesn't quit gracefully, kill it
            goServerProcess->kill();
            goServerProcess->waitForFinished();
        }
        delete goServerProcess;
        goServerProcess = nullptr;
        qDebug() << "Go server stopped.";
    }
}

void startNamePipes(){
    qDebug() << "Starting name pipes";
    pipeServerTranscript = new PipeServer("FloatingNavbarPipeTranscript", nullptr);
    pipeServerHelp = new PipeServer("FloatingNavbarPipeHelp", nullptr);
    pipeServerCapture = new PipeServer("FloatingNavbarPipeCapture", nullptr);
    pipeServerCmd = new PipeServer("FloatingNavbarPipeCommand", nullptr);


    pipeServerTranscript->start();
    pipeServerHelp->start();
    pipeServerCapture->start();
    pipeServerCmd->start();

}

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    startNamePipes();
    Navbar navbar;
    navbar.setPipeServers(pipeServerCmd, pipeServerTranscript, pipeServerHelp,pipeServerCapture);


    startGoServer();

    navbar.show();

    int result = app.exec();

    stopGoServer();

    return result;
}