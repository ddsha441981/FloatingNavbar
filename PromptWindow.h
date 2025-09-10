#ifndef PROMPTWINDOW_H
#define PROMPTWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

class PromptWindow : public QWidget {
    Q_OBJECT
public:
    explicit PromptWindow(QWidget *parent = nullptr);
    void setPromptText(const QString &text);
    void showBelow(QWidget *anchor);



    QString getPromptText() const;

private:
    QTextEdit *promptEdit; // ✅ this is the one in use


};



#endif // PROMPTWINDOW_H
