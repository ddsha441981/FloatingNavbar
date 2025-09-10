//
// Created by deendayal on 12/06/25.

// ModelSelectionDialog.h
#pragma once

#include <QDialog>
#include <QComboBox>
#include <QMap>

class ModelSelectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit ModelSelectionDialog(QWidget *parent = nullptr);
    QString getSelectedModelId() const;

private:
    QComboBox *modelComboBox;
    QMap<QString, QString> modelMap;  // ID → Description
};
