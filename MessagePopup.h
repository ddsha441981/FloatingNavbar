#ifndef MESSAGEPOPUP_H
#define MESSAGEPOPUP_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPropertyAnimation>
#include <QTimer>

class MessagePopup : public QWidget {
    Q_OBJECT
public:
    explicit MessagePopup(QWidget *parent = nullptr);
    void showMessage(const QString &text, QWidget *referenceWidget = nullptr);
    void appendMessage(const QString &text);
    void clearAndHide();

protected:
    void hideEvent(QHideEvent *event) override;

private:
    QScrollArea *scrollArea;
    QLabel *label;
    int maxHeight;
    bool popupShown;
    void scrollToBottom();
    void fadeIn();
    void fadeOut();


    int getLineCount();
    void updateScrollbar();

    QPropertyAnimation *fadeAnimation;
    QTimer *autoHideTimer;
};

#endif // MESSAGEPOPUP_H





// #ifndef MESSAGEPOPUP_H
// #define MESSAGEPOPUP_H
//
// #include <QWidget>
// #include <QLabel>
// #include <QVBoxLayout>
// #include <QScrollArea>
//
// class MessagePopup : public QWidget {
//     Q_OBJECT
// public:
//     explicit MessagePopup(QWidget *parent = nullptr);
//     void showMessage(const QString &text, QWidget *referenceWidget = nullptr);
//     void clearAndHide();
//
// protected:
//     void hideEvent(QHideEvent *event) override;
//
// private:
//     QScrollArea *scrollArea;
//     QLabel *label;
//     int maxHeight;
//     bool popupShown;
//     void scrollToBottom();
// };
//
// #endif // MESSAGEPOPUP_H



// #ifndef MESSAGEPOPUP_H
// #define MESSAGEPOPUP_H
//
// #include <QWidget>
// #include <QLabel>
// #include <QVBoxLayout>
// #include <QScrollArea>
//
// class MessagePopup : public QWidget {
//     Q_OBJECT
// public:
//     explicit MessagePopup(QWidget *parent = nullptr);
//     void showMessage(const QString &text, QWidget *referenceWidget = nullptr);
//
// private:
//     QScrollArea *scrollArea;
//     QLabel *label;
//     int maxHeight;
// };
//
// #endif // MESSAGEPOPUP_H



// #ifndef MESSAGEPOPUP_H
// #define MESSAGEPOPUP_H
//
// #include <QWidget>
// #include <QLabel>
// #include <QVBoxLayout>
// #include <QTimer>
//
// class MessagePopup : public QWidget {
//     Q_OBJECT
// public:
//     explicit MessagePopup(QWidget *parent = nullptr);
//
//     void showMessage(const QString &text, QWidget *referenceWidget = nullptr);
//
// };
//
// #endif // MESSAGEPOPUP_H
