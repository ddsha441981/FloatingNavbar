#ifndef SCREENSHOTRESPONSEWINDOW_H
#define SCREENSHOTRESPONSEWINDOW_H

#include <QTextEdit>

class ScreenshotResponseWindow : public QTextEdit
{
    Q_OBJECT

public:
    explicit ScreenshotResponseWindow(QWidget *parent = nullptr);

    void showLeftOf(QWidget *anchor);
};

#endif // SCREENSHOTRESPONSEWINDOW_H
