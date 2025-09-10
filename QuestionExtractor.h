// QuestionExtractor.h
#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QFutureWatcher>
#include <QRegularExpression>
#include <QSet>
#include <QHash>

struct ClassifiedQuestion {
    QString question;
    QString type;
    QString category;
    double confidence;
    QStringList keywords;

    ClassifiedQuestion() : confidence(0.0) {}
    ClassifiedQuestion(const QString &q, const QString &t, const QString &c = QString(),
                      double conf = 0.0, const QStringList &kw = QStringList())
        : question(q), type(t), category(c), confidence(conf), keywords(kw) {}
};

class QuestionExtractor : public QObject {
    Q_OBJECT

public:
    enum QuestionType {
        WH_QUESTION,
        YES_NO,
        EXPERIENCE,
        SCENARIO,
        TECHNICAL,
        PROJECT,
        BEHAVIORAL,
        CODING,
        SYSTEM_DESIGN,
        OTHER
    };

    enum ComplexityLevel {
        BASIC,
        INTERMEDIATE,
        ADVANCED,
        EXPERT
    };

    explicit QuestionExtractor(QObject *parent = nullptr);
    ~QuestionExtractor();

    // Async usage
    void extractAsync(const QString &transcript);

    // Synchronous usage
    static QList<ClassifiedQuestion> extractQuestions(const QString &transcript);

    // Configuration
    void setMinConfidence(double confidence) { minConfidence = confidence; }
    void setIncludePartialMatches(bool include) { includePartialMatches = include; }

signals:
    void questionsExtracted(const QList<ClassifiedQuestion> &classifiedQuestions);
    void extractionProgress(int percentage);

private slots:
    void onExtractionFinished();

private:
    // Core extraction methods
    static QStringList extractRawQuestions(const QString &text);
    static QList<ClassifiedQuestion> classifyQuestions(const QStringList &questions);

    // Helper methods
    static QString determineQuestionType(const QString &question);
    static QString determineCategory(const QString &question, const QString &type);
    static double calculateConfidence(const QString &question, const QString &type, const QString &category);
    static QStringList extractKeywords(const QString &question);
    static ComplexityLevel determineComplexity(const QString &question);
    static bool isValidQuestion(const QString &text);
    static QString cleanQuestion(const QString &question);

    // Static regex patterns (compiled once)
    static void initializePatterns();
    static QHash<QString, QRegularExpression> patterns;
    static QHash<QString, QSet<QString>> keywords;
    static bool patternsInitialized;

    // Instance variables
    QFutureWatcher<QList<ClassifiedQuestion>> *watcher;
    double minConfidence;
    bool includePartialMatches;
};

Q_DECLARE_METATYPE(ClassifiedQuestion)


// // //QuestionExtractor.h
//
// #pragma once
//
// #include <qfuturewatcher.h>
// #include <QList>
// #include <QObject>
//
// struct ClassifiedQuestion {
//     QString question;
//     QString type; // "WH", "Yes/No", "Experience", "Scenario", etc.
// };
//
// class QuestionExtractor : public QObject {
//     Q_OBJECT
// public:
//     explicit QuestionExtractor(QObject *parent = nullptr);
//     ~QuestionExtractor();
//
//     // Async usage
//     void extractAsync(const QString &transcript);
//
//     // Synchronous usage
//     static QList<ClassifiedQuestion> extractQuestions(const QString &transcript);
//
//     signals:
//         void questionsExtracted(const QList<ClassifiedQuestion> &classifiedQuestions);
//
// private:
//     static QStringList extractRawQuestions(const QString &text);
//     static QList<ClassifiedQuestion> classifyQuestions(const QStringList &questions);
//     QFutureWatcher<QList<ClassifiedQuestion>> *watcher;
// };
