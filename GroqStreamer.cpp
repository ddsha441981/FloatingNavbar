#include "GroqStreamer.h"
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
// #include <QNetworkConfigurationManager>
#include <QSettings>

#include "ModelSelectionDialog.h"

GroqStreamer::GroqStreamer(QObject *parent) : QObject(parent), currentReply(nullptr) {
    networkManager = new QNetworkAccessManager(this);
}

GroqStreamer::~GroqStreamer() {
    if (currentReply) {
        currentReply->deleteLater();
    }
}

void GroqStreamer::startStreaming(const QString &text, const QString &apiKey) {
    QString selectedModelId;
    QSettings settings;
    selectedModelId = settings.value("selectedModel", "llama3-8b-8192").toString();

    qDebug() << "Using model:" << selectedModelId;

    if (currentReply) {
        currentReply->abort();
        currentReply->deleteLater();
        currentReply = nullptr;
    }

    QString urlStr = "https://api.groq.com/openai/v1/chat/completions";
    QUrl url(urlStr);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + apiKey.toUtf8());

    QJsonObject message;
    message["role"] = "user";
    message["content"] = text.trimmed();

    QJsonObject payload;
    payload["model"] = selectedModelId;  //llama3-70b-8192 //
    payload["messages"] = QJsonArray{ message };
    payload["stream"] = true;

    QByteArray jsonData = QJsonDocument(payload).toJson();

    currentReply = networkManager->post(request, jsonData);

    connect(currentReply, &QNetworkReply::readyRead, this, &GroqStreamer::onReadyRead);
    connect(currentReply, &QNetworkReply::finished, this, &GroqStreamer::onFinished);
    connect(currentReply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError code){
        QString customError;

        switch (code) {
            case QNetworkReply::HostNotFoundError:
                customError = "Network error: Host not found. Please check your internet connection.";
                break;
            case QNetworkReply::TimeoutError:
                customError = "Network timeout: The request took too long to complete.";
                break;
            case QNetworkReply::ConnectionRefusedError:
                customError = "Connection refused: The server may be unavailable.";
                break;
            case QNetworkReply::NetworkSessionFailedError:
                customError = "Network session failed: Unable to establish a session.";
                break;
            default:
                customError = "Unexpected error: " + currentReply->errorString();
                break;
        }

        emit streamingFailed(customError);
        currentReply->deleteLater();
        currentReply = nullptr;
    });

    buffer.clear();
}


void GroqStreamer::onReadyRead() {
    if (!currentReply) return;

    QByteArray newData = currentReply->readAll();
    buffer.append(newData);

    QList<QByteArray> messages = buffer.split('\n');
    for (const QByteArray &msg : messages) {
        if (msg.trimmed().isEmpty()) continue;

        if (msg.startsWith("data: ")) {
            QByteArray jsonPart = msg.mid(6);

            if (jsonPart.trimmed() == "[DONE]") continue;

            QJsonParseError err;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonPart, &err);
            if (err.error == QJsonParseError::NoError && jsonDoc.isObject()) {
                QJsonObject root = jsonDoc.object();

                if (root.contains("choices")) {
                    QJsonArray choices = root["choices"].toArray();
                    if (!choices.isEmpty()) {
                        QJsonObject delta = choices[0].toObject()["delta"].toObject();
                        QString token = delta["content"].toString();
                        if (!token.isEmpty()) {
                            emit tokenReceived(token);
                        }
                    }
                }
            }
        }
    }

    buffer.clear();
}

void GroqStreamer::onFinished() {
    if (!currentReply) return;

    int statusCode = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode != 200) {
        QString errorMsg = QString("Echomind API error (%1): %2")
            .arg(statusCode)
            .arg(currentReply->errorString());
        emit streamingFailed(errorMsg);
    } else {
        emit streamingFinished();
    }

    currentReply->deleteLater();
    currentReply = nullptr;
}
