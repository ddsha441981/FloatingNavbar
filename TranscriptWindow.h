#ifndef TRANSCRIPTWINDOW_H
#define TRANSCRIPTWINDOW_H

#include <QTextEdit>

class TranscriptWindow : public QTextEdit {
    Q_OBJECT
public:
    explicit TranscriptWindow(QWidget *parent = nullptr);

    void showRightOf(QWidget *anchor);  // Utility to position relative to navbar


};

#endif // TRANSCRIPTWINDOW_H
