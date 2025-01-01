//
// Created by aashik on 30/12/24.
//

#pragma once
#include <QObject>
#include <QVariantList>

class Controller : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList model MEMBER model NOTIFY modelChanged)

public:
    explicit Controller(QObject *parent = nullptr);

    Q_INVOKABLE void onButtonClicked(const int&);
    Q_INVOKABLE void updateButtonState();

signals:
    void modelChanged();

private:
    void initButtonState();
    void addButton(const QString&, const QString&, const QString&);

    QVariantList model;
    QVariantMap button_state;

    //TODE: TMP Variable Should Moved to MINER Module
    bool miner_running = false;
};
