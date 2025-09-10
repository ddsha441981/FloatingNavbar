#ifndef GROQSTREAMER_H
#define GROQSTREAMER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>

class GroqStreamer : public QObject {
    Q_OBJECT
public:
    explicit GroqStreamer(QObject *parent = nullptr);
    ~GroqStreamer();

    void startStreaming(const QString &text, const QString &apiKey);

    signals:
        void tokenReceived(const QString &token);
    void streamingFinished();
    void streamingFailed(const QString &error);

private slots:
    void onReadyRead();
    void onFinished();

private:
    QNetworkAccessManager *networkManager;
    QNetworkReply *currentReply;
    QByteArray buffer;
};

#endif // GROQSTREAMER_H
