#pragma once
#include <QDialog>

class AudioDeviceSelector;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private:
    AudioDeviceSelector *audioDeviceSelector;
};
