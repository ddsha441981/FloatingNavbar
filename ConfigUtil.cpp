#include "ConfigUtil.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

QString getHomePath() {
    return QDir::homePath();
}

QString readApiKeyFromCustomPath() {
    QString basePath = getHomePath();
    QString configPath = basePath + QDir::separator() + "mytools" + QDir::separator() +
                         "data" + QDir::separator() + "config.txt";

    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open config file at:" << configPath;
        return QString();
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.startsWith("API_KEY=")) {
            QString apiKey = line.mid(QString("API_KEY=").length()).trimmed();
            file.close();
            return apiKey;
        }
    }

    file.close();
    qWarning() << "API_KEY not found in config file.";
    return QString();
}
