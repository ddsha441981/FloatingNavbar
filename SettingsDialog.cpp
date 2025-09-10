#include "SettingsDialog.h"
#include "AudioDeviceSelector.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QMediaDevices>
#include <QAudioDevice>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setWindowFlags(Qt::Popup);
    setStyleSheet("background-color: #222; color: white;");
    setFixedSize(300, 60);  // Match MoreOptionsDialog size

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(20);

    audioDeviceSelector = new AudioDeviceSelector("audioInputDevice", this);
    audioDeviceSelector->setStyleSheet(R"(
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

    // Populate available audio input devices (Qt 6)
    const auto devices = QMediaDevices::audioInputs();
    for (const QAudioDevice &device : devices) {
        qDebug() << "Available:" << device.description();
        audioDeviceSelector->addItem(device.description(), QVariant::fromValue(device));
    }

    layout->addStretch();
    layout->addWidget(audioDeviceSelector);
    layout->addStretch();

    // Close popup when selection changes
    connect(audioDeviceSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &QDialog::accept);
}





// #include "SettingsDialog.h"
// #include "AudioDeviceSelector.h"
//
// #include <QHBoxLayout>
// #include <QDebug>
// #include <QMediaDevices>
// #include <QAudioDevice>
//
// SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
//     setWindowFlags(Qt::Popup);
//     setStyleSheet("background-color: #222; color: white;");
//     setFixedSize(300, 60);  // Same size as MoreOptionsDialog
//
//     QHBoxLayout *layout = new QHBoxLayout(this);
//     layout->setContentsMargins(10, 10, 10, 10);
//     layout->setSpacing(20);
//
//     audioDeviceSelector = new AudioDeviceSelector("audioInputDevice", this);
//     audioDeviceSelector->setStyleSheet(R"(
//         QComboBox {
//             background-color: #444;
//             color: white;
//             padding: 6px 12px;
//             border: none;
//             border-radius: 4px;
//         }
//         QComboBox QAbstractItemView {
//             background-color: #333;
//             color: white;
//             selection-background-color: #555;
//         }
//     )");
//
//     layout->addStretch();
//     layout->addWidget(audioDeviceSelector);
//     layout->addStretch();
//
//     // Print available devices (Qt 6 style)
//     const auto devices = QMediaDevices::audioInputs();
//     for (const QAudioDevice &device : devices) {
//         qDebug() << " -" << device.description();
//     }
//
//     connect(audioDeviceSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &QDialog::accept);
// }





// #include "SettingsDialog.h"
// #include "AudioDeviceSelector.h"
//
// #include <QHBoxLayout>
// #include <QDebug>
// #include <QMediaDevices>
// #include <QAudioDevice>
//
// #include <QAudioDeviceInfo>  // Required for QAudioDeviceInfo
//
// SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
//     setWindowFlags(Qt::Popup);
//     setStyleSheet("background-color: #222; color: white;");
//     setFixedSize(300, 60);  // Same size as MoreOptionsDialog
//
//     QHBoxLayout *layout = new QHBoxLayout(this);
//     layout->setContentsMargins(10, 10, 10, 10);
//     layout->setSpacing(20);
//
//     audioDeviceSelector = new AudioDeviceSelector("audioInputDevice", this);
//     audioDeviceSelector->setStyleSheet(R"(
//         QComboBox {
//             background-color: #444;
//             color: white;
//             padding: 6px 12px;
//             border: none;
//             border-radius: 4px;
//         }
//         QComboBox QAbstractItemView {
//             background-color: #333;
//             color: white;
//             selection-background-color: #555;
//         }
//     )");
//
//     layout->addStretch();
//     layout->addWidget(audioDeviceSelector);
//     layout->addStretch();
//
//     // Print available devices:
//     const auto devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
//
//     // qDebug() << "Available audio input devices:";
//     // for (const QAudioDeviceInfo &device : devices) {
//     //     qDebug() << " -" << device.deviceName();
//     // }
//
//     // Clicking outside closes the popup
//     connect(audioDeviceSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &QDialog::accept);
// }
