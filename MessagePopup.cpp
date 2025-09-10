#include "MessagePopup.h"
#include <QScrollArea>
#include <QFontMetrics>
#include <QApplication>
#include <QScrollBar>
#include <QGraphicsOpacityEffect>

MessagePopup::MessagePopup(QWidget *parent) : QWidget(parent), popupShown(false) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet("background-color: rgba(0, 0, 0, 230); border-radius: 8px;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // Scroll area
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border: none;");
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Initially off

    // Label inside scroll area
    label = new QLabel(this);
    label->setStyleSheet("color: white; font-size: 16px; padding: 10px;");
    label->setWordWrap(true);
    label->setObjectName("popupLabel");

    scrollArea->setWidget(label);
    layout->addWidget(scrollArea);

    // Calculate max height based on 20 lines
    QFontMetrics fm(label->font());
    int lineHeight = fm.lineSpacing(); // includes leading
    maxHeight = (lineHeight * 15) + 20; // 20 lines + padding

    // Fade animation
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opacityEffect);

    fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeAnimation->setDuration(300); // 300ms fade

    // Auto-hide timer
    autoHideTimer = new QTimer(this);
    autoHideTimer->setSingleShot(true);
    connect(autoHideTimer, &QTimer::timeout, this, &MessagePopup::fadeOut);
}

int MessagePopup::getLineCount() {
    // Approximate line count by counting '\n' + 1
    // Could be improved by text layout, but this works fine for simple text
    return label->text().count('\n') + 1;
}

void MessagePopup::showMessage(const QString &text, QWidget *referenceWidget) {
    label->setText(text);

    int lines = getLineCount();
    QFontMetrics fm(label->font());
    int lineHeight = fm.lineSpacing();

    int popupHeight;
    if (lines <= 15) {
        // Grow height to fit all lines (+ padding)
        popupHeight = (lineHeight * lines) + 15;
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        // Fix height to maxHeight (20 lines)
        popupHeight = maxHeight;
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }

    if (!popupShown) {
        if (referenceWidget) {
            int popupWidth = referenceWidget->width();
            int popupX = referenceWidget->x();
            int popupY = referenceWidget->y() + referenceWidget->height() + 10;

            setGeometry(popupX, popupY, popupWidth, popupHeight);
        }

        show();
        fadeIn();
        popupShown = true;
    } else {
        // Update height dynamically if popup already shown
        resize(width(), popupHeight);
    }

    scrollToBottom();
}

void MessagePopup::appendMessage(const QString &text) {
    QString currentText = label->text();
    currentText += "\n" + text;
    label->setText(currentText);

    int lines = getLineCount();
    QFontMetrics fm(label->font());
    int lineHeight = fm.lineSpacing();

    int popupHeight;
    if (lines <= 15) {
        popupHeight = (lineHeight * lines) + 15;
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        popupHeight = maxHeight;
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }

    resize(width(), popupHeight);
    scrollToBottom();
}

void MessagePopup::scrollToBottom() {
    QScrollBar *vScrollBar = scrollArea->verticalScrollBar();
    if (vScrollBar) {
        vScrollBar->setValue(vScrollBar->maximum());
    }
}

void MessagePopup::fadeIn() {
    fadeAnimation->stop();
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->start();
}

void MessagePopup::fadeOut() {
    fadeAnimation->stop();
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);

    connect(fadeAnimation, &QPropertyAnimation::finished, this, [this]() {
        clearAndHide();
    });

    fadeAnimation->start();
}

void MessagePopup::clearAndHide() {
    label->clear();
    hide();
}

void MessagePopup::hideEvent(QHideEvent *event) {
    popupShown = false;
    QWidget::hideEvent(event);
}


