#ifndef MOREOPTIONSDIALOG_H
#define MOREOPTIONSDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

class MoreOptionsDialog : public QDialog {
    Q_OBJECT
public:
    explicit MoreOptionsDialog(QWidget *parent = nullptr);

signals:
    void settingsClicked();
    void contextShortcutClicked();
    void resetEveyThing();
};

#endif // MOREOPTIONSDIALOG_H
