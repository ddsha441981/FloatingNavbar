// QuestionExtractor.cpp
#include "QuestionExtractor.h"
#include <QtConcurrent>
#include <QRegularExpression>
#include <QDebug>
#include <QThread>

// Static member initialization
QHash<QString, QRegularExpression> QuestionExtractor::patterns;
QHash<QString, QSet<QString>> QuestionExtractor::keywords;
bool QuestionExtractor::patternsInitialized = false;

QuestionExtractor::QuestionExtractor(QObject *parent)
    : QObject(parent)
    , watcher(new QFutureWatcher<QList<ClassifiedQuestion>>(this))
    , minConfidence(0.3)
    , includePartialMatches(true)
{
    initializePatterns();

    connect(watcher, &QFutureWatcher<QList<ClassifiedQuestion>>::finished,
            this, &QuestionExtractor::onExtractionFinished);

    // Register metatype for Qt's signal system
    qRegisterMetaType<ClassifiedQuestion>("ClassifiedQuestion");
    qRegisterMetaType<QList<ClassifiedQuestion>>("QList<ClassifiedQuestion>");
}

QuestionExtractor::~QuestionExtractor() {
    if (watcher && watcher->isRunning()) {
        watcher->cancel();
        watcher->waitForFinished();
    }
}

void QuestionExtractor::initializePatterns() {
    if (patternsInitialized) return;

    // Question detection patterns
    patterns["direct_question"] = QRegularExpression(
        R"((?:^|\n|\.\s+)([^.!?]*\?(?:\s*[A-Z])?[^.!?]*?)(?=\.|!|\?|$|\n))",
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption
    );

    patterns["wh_questions"] = QRegularExpression(
        R"(\b(what|who|where|when|why|which|how|whom|whose)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    patterns["yes_no_questions"] = QRegularExpression(
        R"(\b(is|are|was|were|do|does|did|can|could|should|will|would|have|has|had|am|may|might|shall)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    patterns["behavioral_starters"] = QRegularExpression(
        R"(\b(tell me about|describe|explain|walk me through|give me an example|can you share|share with me)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    patterns["scenario_questions"] = QRegularExpression(
        R"(\b(scenario|situation|how would you|what would you do|if you were|suppose|let's say|assume|imagine|hypothetically)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    patterns["experience_questions"] = QRegularExpression(
        R"(\b(have you ever|experience with|worked on|handled|dealt with|your background in|familiar with)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    patterns["technical_indicators"] = QRegularExpression(
        R"(\b(algorithm|complexity|database|framework|api|design pattern|architecture|scalability|performance|optimization|debugging|testing|sql|nosql|microservices|distributed|concurrent|thread|memory|cpu|network|security|authentication|encryption)\b)",
        QRegularExpression::CaseInsensitiveOption
    );

    patterns["coding_indicators"] = QRegularExpression(
        R"(\b(implement|code|write|program|function|method|class|solve|debug|fix|refactor|optimize code|programming|coding|software development)\b)",
        QRegularExpression::CaseInsensitiveOption
    );

    // Add patterns for common real interview speech:
    patterns["conversational_starters"] = QRegularExpression(
        R"(\b(so|well|right|okay|alright|now|actually|let me ask|one more thing)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    patterns["follow_up_questions"] = QRegularExpression(
        R"(\b(going back to|building on|following up on|you mentioned|regarding what you said)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    patterns["clarification_questions"] = QRegularExpression(
        R"(\b(can you clarify|what do you mean by|could you elaborate|can you explain more)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    // Interview transition phrases
    patterns["interview_transitions"] = QRegularExpression(
        R"(\b(moving on|next question|let's switch|changing topics|another question|final question)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    // Probing questions
    patterns["probing_questions"] = QRegularExpression(
        R"(\b(can you dive deeper|tell me more about|expand on that|what else|anything else|what about)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    // Hypothetical scenarios (common in interviews)
    patterns["hypothetical_scenarios"] = QRegularExpression(
        R"(\b(let's say|suppose|imagine|what if|consider this|hypothetical|pretend)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    // Resume/background specific
    patterns["resume_questions"] = QRegularExpression(
        R"(\b(I see you|looking at your|on your resume|your experience at|you worked at|you mentioned)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    // Code review/technical assessment
    patterns["code_review_questions"] = QRegularExpression(
        R"(\b(looking at this code|this implementation|code sample|would you change|optimize this|what's wrong with)\b[^.!?]*[.?!]?)",
        QRegularExpression::CaseInsensitiveOption
    );

    // Initialize keyword sets
    keywords["wh_words"] = {"what", "who", "where", "when", "why", "which", "how", "whom", "whose"};
    keywords["yes_no_words"] = {"is", "are", "was", "were", "do", "does", "did", "can", "could",
                               "should", "will", "would", "have", "has", "had", "am", "may", "might", "shall"};
    keywords["technical_terms"] = {"algorithm", "data structure", "complexity", "database", "framework",
                                  "api", "design pattern", "architecture", "scalability", "performance"};
    keywords["behavioral_terms"] = {"leadership", "teamwork", "conflict", "challenge", "failure",
                                   "success", "communication", "problem solving"};
    keywords["experience_terms"] = {"experience", "background", "worked on", "handled", "familiar", "used"};
    keywords["conversational_terms"] = {"so", "well", "right", "okay", "actually", "now"};
    keywords["follow_up_terms"] = {"going back", "building on", "following up", "you mentioned"};
    keywords["clarification_terms"] = {"clarify", "elaborate", "explain more", "what do you mean"};

    patternsInitialized = true;
}

void QuestionExtractor::extractAsync(const QString &transcript) {
    if (transcript.trimmed().isEmpty()) {
        emit questionsExtracted({});
        return;
    }

    if (watcher->isRunning()) {
        watcher->cancel();
        watcher->waitForFinished();
    }

    QFuture<QList<ClassifiedQuestion>> future = QtConcurrent::run([this, transcript]() {
        return extractQuestions(transcript);
    });

    watcher->setFuture(future);
}

QList<ClassifiedQuestion> QuestionExtractor::extractQuestions(const QString &transcript) {
    if (transcript.trimmed().isEmpty()) {
        return {};
    }

    // Step 1: Extract raw questions
    QStringList rawQuestions = extractRawQuestions(transcript);

    // Step 2: Clean and validate questions
    QStringList validQuestions;
    for (const QString &question : rawQuestions) {
        QString cleaned = cleanQuestion(question);
        if (isValidQuestion(cleaned)) {
            validQuestions.append(cleaned);
        }
    }

    // Step 3: Classify questions
    return classifyQuestions(validQuestions);
}

QStringList QuestionExtractor::extractRawQuestions(const QString &text) {
    QSet<QString> uniqueQuestions; // Use set to avoid duplicates
    QString processedText = text;

    // Normalize text
    processedText.replace(QRegularExpression(R"(\s+)"), " ");
    processedText = processedText.trimmed();

    // Extract different types of questions
    QStringList patternKeys = {"direct_question", "wh_questions", "yes_no_questions",
                              "behavioral_starters", "scenario_questions", "experience_questions",
                              "conversational_starters", "follow_up_questions", "clarification_questions",
                              "interview_transitions", "probing_questions", "hypothetical_scenarios",
                              "resume_questions", "code_review_questions"};

    for (const QString &key : patternKeys) {
        if (!patterns.contains(key)) continue;

        QRegularExpressionMatchIterator iterator = patterns[key].globalMatch(processedText);
        while (iterator.hasNext()) {
            QRegularExpressionMatch match = iterator.next();
            QString question = match.captured(0).trimmed();

            // Clean up the question
            if (question.length() > 10 && question.length() < 500) { // Reasonable length bounds
                uniqueQuestions.insert(question);
            }
        }
    }

    // Convert set back to list
    return QStringList(uniqueQuestions.begin(), uniqueQuestions.end());
}

QList<ClassifiedQuestion> QuestionExtractor::classifyQuestions(const QStringList &questions) {
    QList<ClassifiedQuestion> classified;
    classified.reserve(questions.size());

    for (const QString &question : questions) {
        QString type = determineQuestionType(question);
        QString category = determineCategory(question, type);
        double confidence = calculateConfidence(question, type, category);
        QStringList extractedKeywords = extractKeywords(question);

        classified.append(ClassifiedQuestion(question, type, category, confidence, extractedKeywords));
    }

    // Sort by confidence (highest first)
    std::sort(classified.begin(), classified.end(),
              [](const ClassifiedQuestion &a, const ClassifiedQuestion &b) {
                  return a.confidence > b.confidence;
              });

    return classified;
}

QString QuestionExtractor::determineQuestionType(const QString &question) {
    QString lowered = question.toLower();
    QString firstWord = lowered.section(' ', 0, 0);

    // Remove punctuation from first word
    firstWord.remove(QRegularExpression(R"([^\w])"));

    if (keywords["wh_words"].contains(firstWord)) {
        return "WH";
    }

    if (keywords["yes_no_words"].contains(firstWord)) {
        return "Yes/No";
    }

    // Pattern-based detection
    if (patterns["behavioral_starters"].match(lowered).hasMatch()) {
        return "Behavioral";
    }

    if (patterns["scenario_questions"].match(lowered).hasMatch()) {
        return "Scenario";
    }

    if (patterns["experience_questions"].match(lowered).hasMatch()) {
        return "Experience";
    }

    if (patterns["coding_indicators"].match(lowered).hasMatch()) {
        return "Coding";
    }

    if (patterns["technical_indicators"].match(lowered).hasMatch()) {
        return "Technical";
    }

    if (patterns["follow_up_questions"].match(lowered).hasMatch()) {
        return "Follow-up";
    }

    if (patterns["clarification_questions"].match(lowered).hasMatch()) {
        return "Clarification";
    }

    if (patterns["conversational_starters"].match(lowered).hasMatch()) {
        return "Conversational";
    }

    if (patterns["interview_transitions"].match(lowered).hasMatch()) {
        return "Transition";
    }

    if (patterns["probing_questions"].match(lowered).hasMatch()) {
        return "Probing";
    }

    if (patterns["hypothetical_scenarios"].match(lowered).hasMatch()) {
        return "Hypothetical";
    }

    if (patterns["resume_questions"].match(lowered).hasMatch()) {
        return "Resume";
    }

    if (patterns["code_review_questions"].match(lowered).hasMatch()) {
        return "Code Review";
    }

    return "Other";
}

QString QuestionExtractor::determineCategory(const QString &question, const QString &type) {
    QString lowered = question.toLower();

    // Technical subcategories
    if (type == "Technical" || type == "Coding") {
        if (lowered.contains(QRegularExpression(R"(\b(algorithm|sorting|searching|complexity)\b)"))) {
            return "Algorithms";
        }
        if (lowered.contains(QRegularExpression(R"(\b(database|sql|nosql|query)\b)"))) {
            return "Database";
        }
        if (lowered.contains(QRegularExpression(R"(\b(system|architecture|design|scalability)\b)"))) {
            return "System Design";
        }
        if (lowered.contains(QRegularExpression(R"(\b(java|python|javascript|cpp|programming)\b)"))) {
            return "Programming Languages";
        }
        if (lowered.contains(QRegularExpression(R"(\b(framework|library|api|sdk)\b)"))) {
            return "Frameworks/APIs";
        }
        return "General Technical";
    }

    // Behavioral subcategories
    if (type == "Behavioral" || type == "Experience") {
        if (lowered.contains(QRegularExpression(R"(\b(leadership|lead|manage|team)\b)"))) {
            return "Leadership";
        }
        if (lowered.contains(QRegularExpression(R"(\b(conflict|disagreement|difficult)\b)"))) {
            return "Conflict Resolution";
        }
        if (lowered.contains(QRegularExpression(R"(\b(challenge|problem|issue|bug)\b)"))) {
            return "Problem Solving";
        }
        if (lowered.contains(QRegularExpression(R"(\b(project|delivery|deadline)\b)"))) {
            return "Project Management";
        }
        return "General Behavioral";
    }

    // Handle new conversational types
    if (type == "Follow-up") {
        // Determine what the follow-up is about
        if (lowered.contains(QRegularExpression(R"(\b(technical|code|algorithm|system)\b)"))) {
            return "Technical Follow-up";
        }
        if (lowered.contains(QRegularExpression(R"(\b(experience|project|team|work)\b)"))) {
            return "Experience Follow-up";
        }
        return "General Follow-up";
    }

    if (type == "Clarification") {
        return "Clarification Request";
    }

    if (type == "Conversational") {
        // Try to determine the underlying question type
        if (lowered.contains(QRegularExpression(R"(\b(technical|algorithm|code)\b)"))) {
            return "Conversational Technical";
        }
        if (lowered.contains(QRegularExpression(R"(\b(experience|background|worked)\b)"))) {
            return "Conversational Experience";
        }
        return "General Conversational";
    }

    if (type == "Transition") {
        return "Interview Transition";
    }

    if (type == "Probing") {
        return "Probing Question";
    }

    if (type == "Hypothetical") {
        return "Hypothetical Scenario";
    }

    if (type == "Resume") {
        return "Resume Discussion";
    }

    if (type == "Code Review") {
        return "Code Review";
    }

    return "General";
}

double QuestionExtractor::calculateConfidence(const QString &question, const QString &type, const QString &category) {
    double confidence = 0.5; // Base confidence

    // Length factor (reasonable questions have moderate length)
    int length = question.length();
    if (length >= 20 && length <= 200) {
        confidence += 0.2;
    } else if (length < 10 || length > 300) {
        confidence -= 0.2;
    }

    // Question mark presence
    if (question.contains('?')) {
        confidence += 0.2;
    }

    // Strong type indicators
    if (type != "Other") {
        confidence += 0.2;
    }

    // Category specificity
    if (category != "General" && !category.isEmpty()) {
        confidence += 0.1;
    }

    // Keyword richness
    QStringList words = question.split(' ', Qt::SkipEmptyParts);
    int technicalWords = 0;
    for (const QString &word : words) {
        if (keywords["technical_terms"].contains(word.toLower())) {
            technicalWords++;
        }
    }

    if (technicalWords > 0) {
        confidence += qMin(0.2, technicalWords * 0.05);
    }

    return qBound(0.0, confidence, 1.0);
}

QStringList QuestionExtractor::extractKeywords(const QString &question) {
    QSet<QString> keywordSet;
    QString lowered = question.toLower();

    // Extract technical keywords
    for (const QString &term : keywords["technical_terms"]) {
        if (lowered.contains(term)) {
            keywordSet.insert(term);
        }
    }

    // Extract behavioral keywords
    for (const QString &term : keywords["behavioral_terms"]) {
        if (lowered.contains(term)) {
            keywordSet.insert(term);
        }
    }

    // Extract important nouns and verbs using simple heuristics
    QStringList words = question.split(QRegularExpression(R"(\W+)"), Qt::SkipEmptyParts);
    for (const QString &word : words) {
        if (word.length() > 4 && !word.toLower().contains(QRegularExpression(R"(\b(the|and|or|but|in|on|at|to|for|of|with|by)\b)"))) {
            keywordSet.insert(word.toLower());
        }
    }

    return QStringList(keywordSet.begin(), keywordSet.end());
}

bool QuestionExtractor::isValidQuestion(const QString &text) {
    if (text.length() < 5 || text.length() > 1000) {
        return false;
    }

    // Must contain at least one alphabetic character
    if (!text.contains(QRegularExpression(R"([a-zA-Z])"))) {
        return false;
    }

    // Should have at least 2 words
    QStringList words = text.split(QRegularExpression(R"(\s+)"), Qt::SkipEmptyParts);
    if (words.size() < 2) {
        return false;
    }

    return true;
}

QString QuestionExtractor::cleanQuestion(const QString &question) {
    QString cleaned = question.trimmed();

    // Remove multiple spaces
    cleaned.replace(QRegularExpression(R"(\s+)"), " ");

    // Remove leading/trailing punctuation except question marks
    cleaned.remove(QRegularExpression(R"(^[^\w?]+|[^\w?]+$)"));

    // Ensure proper capitalization
    if (!cleaned.isEmpty()) {
        cleaned[0] = cleaned[0].toUpper();
    }

    return cleaned;
}

void QuestionExtractor::onExtractionFinished() {
    if (watcher->isCanceled()) {
        return;
    }

    QList<ClassifiedQuestion> results = watcher->result();

    // Filter by minimum confidence if set
    if (minConfidence > 0.0) {
        QList<ClassifiedQuestion> filtered;
        for (const ClassifiedQuestion &q : results) {
            if (q.confidence >= minConfidence) {
                filtered.append(q);
            }
        }
        results = filtered;
    }

    emit questionsExtracted(results);
}