//
// Created by aashik on 31/12/24.
//

#pragma once
#include <QObject>
#include <QVariantList>

#include <Messenger/receiver.h>

class SystemStats : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList model MEMBER model NOTIFY modelChanged)
    Q_PROPERTY(QString memoryName MEMBER memoryName NOTIFY memoryNameChanged)
    Q_PROPERTY(QString memoryKey MEMBER memoryKey NOTIFY memoryKeyChanged)

public:
    explicit SystemStats(QObject *parent = nullptr);

signals:
    void memoryNameChanged();
    void memoryKeyChanged();
    void modelChanged();

private:
    void InitMessageReceiver();

    std::unique_ptr<Messenger::Receiver> receiver;
    std::unordered_map<QString, int> modelIdx;
    QVariantList model;
    QString memoryKey, memoryName;
    bool memoryKeyFlag = false, memoryNameFlag = false;
};
