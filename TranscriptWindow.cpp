#include "TranscriptWindow.h"

#include <qabstractanimation.h>
#include <QPropertyAnimation>

TranscriptWindow::TranscriptWindow(QWidget *parent)
    : QTextEdit(nullptr)  // Top-level window, not a child of parent
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setReadOnly(true);
    setFixedSize(500, 160);

    setStyleSheet(R"(
        QTextEdit {
             background-color: rgba(0, 0, 0, 230);
             color: white;
             font-size: 16px;
             border-radius: 8px;
             padding: 10px;
             border: none;
         }
    )");

    hide();  // start hidden
}

void TranscriptWindow::showRightOf(QWidget *anchor) {
    // const int margin = 10;  // 10 pixels gap between navbar and transcript window
    // if (anchor) {
    //     int x = anchor->x() + anchor->width() + margin;
    //     int y = anchor->y();
    //     move(x, y);
    // }
    // show();
    // raise();
    const int margin = 10;
    if (!anchor) return;

    int finalX = anchor->x() + anchor->width() + margin;
    int finalY = anchor->y();

    // Start slightly offscreen or offset
    int startX = finalX + 100;

    move(startX, finalY);  // Move to start position
    show();
    raise();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(500);  // ms
    animation->setStartValue(QPoint(startX, finalY));
    animation->setEndValue(QPoint(finalX, finalY));
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
