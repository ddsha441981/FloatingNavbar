// ScreenshotResponseWindow.cpp

#include "ScreenshotResponseWindow.h"

#include <qabstractanimation.h>
#include <QPropertyAnimation>

ScreenshotResponseWindow::ScreenshotResponseWindow(QWidget *parent)
    : QTextEdit(nullptr)  // top-level window, no parent
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setReadOnly(true);
    setFixedSize(500, 260);

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

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    hide();  // start hidden
}

void ScreenshotResponseWindow::showLeftOf(QWidget *anchor) {
    const int margin = 10;
    if (!anchor)
        return;

    int finalX = anchor->x() - width() - margin;
    int finalY = anchor->y();

    // Prevent off-screen
    if (finalX < 0) finalX = 0;
    if (finalY < 0) finalY = 0;

    // Start offset (offscreen left)
    int startX = finalX - 100;
    if (startX < 0) startX = 0;

    move(startX, finalY);  // Start pos
    show();
    raise();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(500);  // ms
    animation->setStartValue(QPoint(startX, finalY));
    animation->setEndValue(QPoint(finalX, finalY));
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    // const int margin = 10;
    // if (!anchor)
    //     return;
    //
    // int x = anchor->x() - width() - margin;
    // int y = anchor->y();
    //
    // // Prevent off-screen
    // if (x < 0) x = 0;
    // if (y < 0) y = 0;
    //
    // move(x, y);
    // show();
    // raise();
}


