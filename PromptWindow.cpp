#include "PromptWindow.h"

PromptWindow::PromptWindow(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(500, 160);  // Same size as TranscriptWindow

    setStyleSheet(R"(
        QWidget {
            background-color: rgba(0, 0, 0, 230);
            border-radius: 8px;
        }
        QTextEdit {
            background-color: #111;
            color: white;
            font-size: 14px;
            padding: 10px;
            border: none;
            border-radius: 4px;
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);

    promptEdit = new QTextEdit(this);
    promptEdit->setPlaceholderText("Enter your custom prompt here...");
    layout->addWidget(promptEdit);
}


QString PromptWindow::getPromptText() const {
    return promptEdit->toPlainText().trimmed();
}

void PromptWindow::setPromptText(const QString& text) {
    if (promptEdit) {
        promptEdit->setPlainText(text);
    }
}


