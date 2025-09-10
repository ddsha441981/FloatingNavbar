#include "ModelSelectionDialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QComboBox>
#include <QLabel>

ModelSelectionDialog::ModelSelectionDialog(QWidget *parent) : QDialog(parent) {
    setWindowFlags(Qt::Popup);
    setStyleSheet("background-color: #222; color: white;");
    setFixedSize(400, 120);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    QLabel *label = new QLabel("Choose Model:", this);
    label->setStyleSheet("color: white; font-weight: bold;");
    layout->addWidget(label);

    modelComboBox = new QComboBox(this);
    modelComboBox->setStyleSheet(R"(
        QComboBox {
            background-color: #444;
            color: white;
            padding: 6px 12px;
            border: none;
            border-radius: 4px;
        }
        QComboBox QAbstractItemView {
            background-color: #333;
            color: white;
            selection-background-color: #555;
        }
    )");

    // 🔑 ID → Description mapping  //llama3-70b-8192   //llama3-8b-8192
    modelMap = {
        {"llama3-8b-8192",          "Default Model | ⚡⚡⚡⚡ | 🧠🧠🧠"},
        {"llama-3.3-70b-versatile", "Model 2 | ⚡⚡⚡ | 🧠🧠🧠🧠🧠"},
        {"gemma-7b-it",             "Model 3 | ⚡⚡⚡  | 🧠🧠"},
        // {"mistral-saba-24b",        "Model 4 | ⚡⚡⚡⚡ | 🧠🧠🧠🧠"},
    };

    // Add all items to combo box
    for (auto it = modelMap.cbegin(); it != modelMap.cend(); ++it) {
        modelComboBox->addItem(it.value(), it.key());  // visible, internal
    }

    layout->addWidget(modelComboBox);

    // Restore last selection
    QSettings settings;
    QString lastModel = settings.value("selectedModel", "llama-3.3-70b-versatile").toString();
    int index = modelComboBox->findData(lastModel);
    if (index != -1)
        modelComboBox->setCurrentIndex(index);

    QPushButton *selectButton = new QPushButton("Select", this);
    selectButton->setStyleSheet("background-color: #444; color: white; padding: 6px 12px;");
    layout->addWidget(selectButton);

    connect(selectButton, &QPushButton::clicked, [this]() {
        // Save to QSettings
        QSettings settings;
        settings.setValue("selectedModel", modelComboBox->currentData().toString());
        accept();  // close with success
    });
}

QString ModelSelectionDialog::getSelectedModelId() const {
    return modelComboBox->currentData().toString();  // Return selected model ID
}
