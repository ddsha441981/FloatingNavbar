#pragma once

#include <QComboBox>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QSettings>
#include <QDebug>

Q_DECLARE_METATYPE(QAudioDevice)

class AudioDeviceSelector : public QComboBox {
    Q_OBJECT

public:
    explicit AudioDeviceSelector(const QString &settingsKey, QWidget *parent = nullptr)
        : QComboBox(parent), settingsKey(settingsKey) {
        QSettings settings;
        QByteArray savedDeviceId = settings.value(settingsKey).toByteArray();

        devices = QMediaDevices::audioInputs();

        int defaultIndex = -1;

        for (int i = 0; i < devices.size(); ++i) {
            const QAudioDevice &device = devices[i];
            QString label = device.description();

            if (label.contains("microphone", Qt::CaseInsensitive)) label = "🎙️ " + label;
            else if (label.contains("speaker", Qt::CaseInsensitive)) label = "🔊 " + label;
            else label = "🎛️ " + label;

            addItem(label);
            setItemData(i, QVariant::fromValue(device));

            if (device.id() == savedDeviceId) {
                defaultIndex = i;
            }
        }

        // Fallback to default input if saved one not found
        if (defaultIndex == -1) {
            QAudioDevice defaultDevice = QMediaDevices::defaultAudioInput();
            for (int i = 0; i < devices.size(); ++i) {
                if (devices[i].id() == defaultDevice.id()) {
                    defaultIndex = i;
                    break;
                }
            }
        }

        if (defaultIndex >= 0) {
            setCurrentIndex(defaultIndex);
        }

        connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged),
           this, [this](int index) {
               if (index >= 0 && index < devices.size()) {
                   QSettings settings;
                   settings.setValue(this->settingsKey, devices[index].id());
                   qDebug() << "Selected audio device:" << devices[index].description();
               }
           });

    }

    QAudioDevice selectedDevice() const {
        int index = currentIndex();
        if (index >= 0 && index < devices.size()) {
            return devices[index];
        }
        return QMediaDevices::defaultAudioInput();
    }

    QString getSettingsKey() const {
        return settingsKey;
    }

private:
    QString settingsKey;
    QList<QAudioDevice> devices;
};



// #pragma once
//
// #include <QComboBox>
// #include <QAudioDevice>
// #include <QMediaDevices>
// #include <QSettings>
// #include <QDebug>
//
// class AudioDeviceSelector : public QComboBox {
//     Q_OBJECT
//
// public:
//     AudioDeviceSelector(const QString &settingsKey, QWidget *parent = nullptr)
//         : QComboBox(parent), settingsKey(settingsKey)
//     {
//         refreshDevices();
//
//         // Load saved device from QSettings
//         QSettings settings("Echomind", "FloatingNavbar");
//         QString savedDeviceId = settings.value(settingsKey).toString();
//
//         // Find corresponding index by device ID
//         int index = findData(savedDeviceId);
//         if (index >= 0) {
//             setCurrentIndex(index);
//         }
//
//         connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AudioDeviceSelector::onDeviceChanged);
//     }
//
//     void refreshDevices() {
//         clear();
//
//         const auto devices = QMediaDevices::audioInputs();
//
//         for (const QAudioDevice &device : devices) {
//             QString name = device.description();
//             QString label;
//
//             if (name.contains("monitor", Qt::CaseInsensitive)) {
//                 label = "🔊 System Audio (Monitor)";
//             } else if (name.contains("analog-stereo", Qt::CaseInsensitive) || name.contains("input", Qt::CaseInsensitive)) {
//                 label = "🎙️ Microphone (Analog Stereo)";
//             } else if (device == QMediaDevices::defaultAudioInput()) {
//                 label = "🎛️ Default Device";
//             } else {
//                 label = name;
//             }
//
//             // Store device ID for future retrieval
//             addItem(label, device.id());
//         }
//     }
//
//     QAudioDevice selectedDevice() const {
//         QByteArray deviceId = currentData().toByteArray();
//
//         for (const QAudioDevice &device : QMediaDevices::audioInputs()) {
//             if (device.id() == deviceId) {
//                 return device;
//             }
//         }
//
//         qWarning() << "[AudioDeviceSelector] Saved device not found, using default input.";
//         return QMediaDevices::defaultAudioInput();
//     }
//
// private slots:
//     void onDeviceChanged(int index) {
//         QByteArray deviceId = itemData(index).toByteArray();
//         QSettings settings("Echomind", "FloatingNavbar");
//         settings.setValue(settingsKey, deviceId);
//     }
//
// private:
//     QString settingsKey;
// };




// #pragma once
//
// #include <QComboBox>
// #include <QAudioDeviceInfo>
// #include <QSettings>
// #include <QDebug>
//
// class AudioDeviceSelector : public QComboBox {
//     Q_OBJECT
//
// public:
//     AudioDeviceSelector(const QString &settingsKey, QWidget *parent = nullptr)
//         : QComboBox(parent), settingsKey(settingsKey)
//     {
//         refreshDevices();
//
//         // Load saved device from QSettings
//         QSettings settings("Echomind", "FloatingNavbar");
//         QString savedDeviceName = settings.value(settingsKey).toString();
//
//         // Find corresponding index by data (raw device name)
//         int index = findData(savedDeviceName);
//         if (index >= 0) {
//             setCurrentIndex(index);
//         }
//
//         connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AudioDeviceSelector::onDeviceChanged);
//     }
//
//     void refreshDevices() {
//         clear();
//
//         const auto devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
//         // qDebug() << "[AudioDeviceSelector] Available audio input devices:";
//         for (const QAudioDeviceInfo &device : devices) {
//             QString name = device.deviceName();
//             QString label;
//
//             // Basic label improvement
//             if (name.contains("monitor", Qt::CaseInsensitive)) {
//                 label = "🔊 System Audio (Monitor)";
//             } else if (name.contains("analog-stereo", Qt::CaseInsensitive) || name.contains("input", Qt::CaseInsensitive)) {
//                 label = "🎙️ Microphone (Analog Stereo)";
//             } else if (name == "default") {
//                 label = "🎛️ Default Device";
//             } else {
//                 label = name;  // fallback: show raw name
//             }
//
//             // qDebug() << " -" << name;
//
//             // Add item → label is shown, deviceName is saved as data
//             addItem(label, name);
//         }
//     }
//
//     QAudioDeviceInfo selectedDevice() const {
//         QString savedName = currentData().toString();
//         const auto devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
//
//         for (const QAudioDeviceInfo &device : devices) {
//             if (device.deviceName() == savedName) {
//                 return device;
//             }
//         }
//
//         qWarning() << "[AudioDeviceSelector] Saved device not found, falling back to default input device.";
//         return QAudioDeviceInfo::defaultInputDevice();
//     }
//
// private slots:
//     void onDeviceChanged(int index) {
//         QString selectedDeviceName = itemData(index).toString();
//         QSettings settings("Echomind", "FloatingNavbar");
//         settings.setValue(settingsKey, selectedDeviceName);
//
//         // qDebug() << "[AudioDeviceSelector] Selected device saved:" << selectedDeviceName;
//     }
//
// private:
//     QString settingsKey;
// };
