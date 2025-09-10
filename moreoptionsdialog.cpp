
//MoreOptionsDialog.cpp
#include "moreoptionsdialog.h"
#include <QSpacerItem>
#include <QSizePolicy>

MoreOptionsDialog::MoreOptionsDialog(QWidget *parent) : QDialog(parent) {
    setWindowFlags(Qt::Popup);
    setStyleSheet("background-color: #222; color: white;");
    setFixedSize(300, 60);  // Adjust as needed

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(20); // Equal gap between buttons

    QPushButton *btnSettings = new QPushButton("Settings", this);
    QPushButton *btnContext = new QPushButton("Model", this);
    QPushButton *btnReset = new QPushButton("Reset", this);

    btnSettings->setStyleSheet("background-color: #444; color: white; padding: 6px 12px;");
    btnContext->setStyleSheet("background-color: #444; color: white; padding: 6px 12px;");
    btnReset->setStyleSheet("background-color: #444; color: white; padding: 6px 12px;");

    // Use stretch to equally space
    layout->addStretch();
    layout->addWidget(btnSettings);
    layout->addStretch();
    layout->addWidget(btnContext);
    layout->addStretch();
    layout->addWidget(btnReset);
    layout->addStretch();

    connect(btnSettings, &QPushButton::clicked, this, &MoreOptionsDialog::settingsClicked);
    connect(btnContext, &QPushButton::clicked, this, &MoreOptionsDialog::contextShortcutClicked);
    connect(btnReset, &QPushButton::clicked, this, &MoreOptionsDialog::resetEveyThing);
}
