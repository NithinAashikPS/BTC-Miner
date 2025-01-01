//
// Created by aashik on 30/12/24.
//

#include <UI/controller.h>
#include <QVariantMap>
#include <QCoreApplication>

#include <BitCoinCore/bitcoincore.h>
#include <System/systemstatlistener.h>

Controller::Controller(QObject *parent) : QObject(parent) {

    initButtonState();
    addButton("shutdown", "\ue8ac", "#FF4C4C");
    addButton("restart", "\uf053", "#FFEB3B");
    addButton("start_mining", "\ue037", "#4CAF50");
    addButton("stop_mining", "\ue047", "#2196F3");
}

void Controller::initButtonState() {

    button_state["shutdown"] = BitCoinCore::serverStarted;
    button_state["restart"] = BitCoinCore::serverStarted;
    button_state["start_mining"] = !miner_running && BitCoinCore::serverStarted;
    button_state["stop_mining"] = miner_running;
}

void Controller::updateButtonState() {

    initButtonState();
    QVariantList tmp_model = model;
    model.clear();
    for (const auto& model_item: tmp_model) {
        QVariantMap button = model_item.toMap();
        button["enabled"] = button_state[button["id"].toString()];
        model.append(button);
    }
    emit modelChanged();
}

void Controller::addButton(const QString& id, const QString& icon, const QString& color) {
    QVariantMap button;
    button["id"] = id;
    button["icon"] = icon;
    button["color"] = color;
    button["enabled"] = button_state[id];
    model.append(button);
    emit modelChanged();
}

void Controller::onButtonClicked(const int& idx) {
    QVariantMap button = model.at(idx).toMap();
    const QString id = button["id"].toString();
    if (id.contains("mining")) {
        miner_running =! miner_running;
        updateButtonState();
        return;
    }
    if (id.contains("shutdown")) {
        BitCoinCore::Stop();
        SystemStatListener::Stop();
        QCoreApplication::quit();
        return;
    }
    if (id.contains("restart")) {
        BitCoinCore::Restart();
        return;
    }
}
