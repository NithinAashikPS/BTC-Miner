//
// Created by aashik on 01/01/25.
//

#pragma once
#include <QObject>
#include <QVariantList>

#include <Messenger/receiver.h>

class MessageReceiver : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantMap message MEMBER message NOTIFY messageChanged)
    Q_PROPERTY(QString memoryName MEMBER memoryName NOTIFY memoryNameChanged)
    Q_PROPERTY(QString memoryKey MEMBER memoryKey NOTIFY memoryKeyChanged)

public:
    explicit MessageReceiver(QObject *parent = nullptr);

    signals:
        void memoryNameChanged();
    void memoryKeyChanged();
    void messageChanged();

private:
    void InitMessageReceiver();

    std::unique_ptr<Messenger::Receiver> receiver;
    QVariantMap message;
    QString memoryKey, memoryName;
    bool memoryKeyFlag = false, memoryNameFlag = false;
};

