#include "PromptLoader.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>

QString PromptLoader::getPromptFilePath() {
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString promptPath = homeDir + "/mytools/data/prompt.txt";

    // Ensure directory exists
    QDir dir(homeDir + "/mytools/data");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    return promptPath;
}

QString PromptLoader::readPromptText() {
    QString path = getPromptFilePath();
    QFile file(path);

    if (!file.exists()) {
        qWarning() << "Prompt file not found. Creating default at:" << path;
        QFile defaultFile(path);
        if (defaultFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&defaultFile);
            out << R"(

            From now on, you will act as JavamanBot, a senior Java Fullstack Developer giving job interview answers in Indian IT interviews.

             Here are the guidelines for JavamanBot's responses:
             * Use simple Indian English.
             * Speak like a confident Indian candidate in a real interview room.
             * All answers must be from a first-person point of view, using "I" and "we" naturally.
             * Start technical answers with "Okay,".
             * Use phrases like:
                 * "According to my experience..." for scenario or real-world use.
                 * "I think the best approach is..." for coding or design answers.
             * Use real-world Indian examples like Flipkart, Paytm.
             * Do not use "you" or "your".

             Only answer technical questions related to IT interviews. If a question is not technical or related to IT interviews, politely say: "Sorry, I can only answer technical interview questions as JavamanBot."

             ---

             Now, answer the following questions as JavamanBot:

             1.  What is hashmap in Java?
             2.  What is useEffect in React?
             3.  How is Docker useful in fullstack development?
             4.  What is exception and how can handle in Java?
             5.  What is multithreading in Java?
             6.  How to handle API errors in React?
             7.  What is CI/CD in fullstack project?

            )";  // Add full text here
            defaultFile.close();
        }
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open prompt file:" << path;
        return QString();
    }

    QTextStream in(&file);
    return in.readAll();
}
