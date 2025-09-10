#include "audiowsclient.h"

#include <qabstractsocket.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

//AudioWebSocketClient::AudioWebSocketClient(const QUrl &url, QObject *parent)
    //: QObject(parent) {
    //connect(&m_webSocket, &QWebSocket::connected, this, &AudioWebSocketClient::onConnected);
    //connect(&m_webSocket, &QWebSocket::disconnected, this, &AudioWebSocketClient::onDisconnected);
    //connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &AudioWebSocketClient::onTextMessageReceived);
    //connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),this, &AudioWebSocketClient::onError);
    //m_webSocket.open(url);
//}
    AudioWebSocketClient::AudioWebSocketClient(const QUrl &url, QObject *parent)
        : QObject(parent) {
        connect(&m_webSocket, &QWebSocket::connected, this, &AudioWebSocketClient::onConnected);
        connect(&m_webSocket, &QWebSocket::disconnected, this, &AudioWebSocketClient::onDisconnected);
        connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &AudioWebSocketClient::onTextMessageReceived);
        connect(&m_webSocket, &QWebSocket::errorOccurred, this, &AudioWebSocketClient::onError);

        m_webSocket.open(url);
    }




bool AudioWebSocketClient::isConnected() const {
    return m_webSocket.state() == QAbstractSocket::ConnectedState;
}


void AudioWebSocketClient::sendAudioData(const QByteArray &pcmData) {
    if (m_webSocket.state() == QAbstractSocket::ConnectedState) {
        m_webSocket.sendBinaryMessage(pcmData);
    }
}

void AudioWebSocketClient::sendCloseMicrophone() {
    QJsonObject closeMsg;
    closeMsg["type"] = "closeMicrophone";
    QJsonDocument doc(closeMsg);
    m_webSocket.sendTextMessage(doc.toJson(QJsonDocument::Compact));
}

void AudioWebSocketClient::onConnected() {
    qDebug() << "WebSocket connected";
}

void AudioWebSocketClient::onDisconnected() {
    qDebug() << "WebSocket disconnected";
}

void AudioWebSocketClient::onTextMessageReceived(const QString &message) {
    qDebug() << "Transcription received:" << message;
    emit transcriptionReceived(message);
}

void AudioWebSocketClient::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);
    qWarning() << "WebSocket error:" << m_webSocket.errorString();
}
