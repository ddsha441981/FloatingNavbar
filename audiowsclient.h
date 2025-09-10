#ifndef AUDIOWSCLIENT_H
#define AUDIOWSCLIENT_H

#include <qabstractsocket.h>
#include <QObject>
#include <QWebSocket>
#include <QUrl>

class AudioWebSocketClient : public QObject {
    Q_OBJECT
public:
    explicit AudioWebSocketClient(const QUrl &url, QObject *parent = nullptr);
    void sendAudioData(const QByteArray &pcmData);
    void sendCloseMicrophone();

    // Add this method to check connection status
    bool isConnected() const;

    signals:
        void transcriptionReceived(const QString &text);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);

private:
    QWebSocket m_webSocket;
};

#endif // AUDIOWSCLIENT_H
