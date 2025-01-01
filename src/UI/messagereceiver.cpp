//
// Created by aashik on 01/01/25.
//

#include <UI/messagereceiver.h>
#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>

MessageReceiver::MessageReceiver(QObject *parent) : QObject(parent) {
    connect(this, &MessageReceiver::memoryNameChanged, this, [&]() {
        if (memoryNameFlag)
            throw std::runtime_error("Memory NAME is not changeable.");
        memoryNameFlag = true;
        if (!memoryKey.isEmpty())
            InitMessageReceiver();
    });
    connect(this, &MessageReceiver::memoryKeyChanged, this, [&]() {
        if (memoryKeyFlag)
            throw std::runtime_error("Memory KEY is not changeable.");
        memoryKeyFlag = true;
        if (!memoryName.isEmpty())
            InitMessageReceiver();
    });
}

void MessageReceiver::InitMessageReceiver() {
    receiver = std::make_unique<Messenger::Receiver>(memoryName.toStdString(), memoryKey.toStdString());
    receiver->getMessage([&](const std::string &result) {
        message = QJsonDocument::fromJson(QString::fromStdString(result).toUtf8()).object().toVariantMap();
        emit messageChanged();
    });
}
