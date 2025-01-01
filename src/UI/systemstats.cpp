//
// Created by aashik on 31/12/24.
//

#include <UI/systemstats.h>
#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>

SystemStats::SystemStats(QObject *parent) : QObject(parent) {
    connect(this, &SystemStats::memoryNameChanged, this, [&]() {
        if (memoryNameFlag)
            throw std::runtime_error("Memory NAME is not changeable.");
        memoryNameFlag = true;
        if (!memoryKey.isEmpty())
            InitMessageReceiver();
    });
    connect(this, &SystemStats::memoryKeyChanged, this, [&]() {
        if (memoryKeyFlag)
            throw std::runtime_error("Memory KEY is not changeable.");
        memoryKeyFlag = true;
        if (!memoryName.isEmpty())
            InitMessageReceiver();
    });
}

void SystemStats::InitMessageReceiver() {
    receiver = std::make_unique<Messenger::Receiver>(memoryName.toStdString(), memoryKey.toStdString());
    receiver->getMessage([&](const std::string &message) {
        auto modelItem = QJsonDocument::fromJson(QString::fromStdString(message).toUtf8()).object().toVariantMap();
        QString name = modelItem["name"].toString();
        if (modelIdx.find(name) == modelIdx.end()) {
            modelIdx[name] = model.size();
            model.append(modelItem);
        } else {
            model.replace(modelIdx.at(name), modelItem);
        }
        emit modelChanged();
    });
}
