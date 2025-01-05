//
// Created by aashik on 01/01/25.
//

#include <UI/bitcoincore_ui.h>

#include <QtConcurrent>
#include <QJsonObject>

BitcoinCoreModule::BitcoinCoreModule(QObject *parent) : QObject(parent) {
    messenger = std::make_unique<Messenger::Sender>("command_response", "BTC_CLI");
    _callback = [&](const std::string &result) {
        const auto &message = QJsonDocument::fromJson(QString::fromStdString(result).toUtf8()).object();
        std::string final_message;
        if (!message["error"].isNull()) {
            final_message = "Error: ";
            final_message += message["error"].toObject()["message"].toString().toStdString();
        } else {
            final_message = "Success: ";
            if (message["result"].isObject()) {
                final_message += QJsonDocument(message["result"].toObject()).toJson().toStdString();
            } else {
                final_message += message["result"].toString().toStdString();
            }
        }
        if (!final_message.empty()) {
            messenger->putMessage(final_message);
        }
    };
}

void BitcoinCoreModule::runCommand(const QString &command) {

    if (removeCount)
        commands.remove(commands.count()-removeCount-1, removeCount);
    currentIdx = commands.count();
    removeCount = 0;
    commands.emplaceBack(command);
    if (command == "clear")
        return;

    auto localRun = QtConcurrent::run([&](const QString &cmd) {
        const auto& args = cmd.split(" ");
        messenger->putMessage("> " + cmd.toStdString());
        std::string params = "[";
        for (int i = 1; i < args.count(); i++) {
            params += args.at(i).toStdString();
            if (i != args.count() - 1)
                params += ", ";
        }
        params += "]";
        BitCoinCore::call(args.at(0).toStdString(), _callback, params);
    }, command);

}

QString BitcoinCoreModule::decreamentCommand() {
    if (currentIdx == 0)
        return commands.at(currentIdx);
    currentIdx -= 1;
    removeCount += 1;
    return commands.at(currentIdx);
}

QString BitcoinCoreModule::increamentCommand() {
    if (currentIdx == commands.count()-1)
        return commands.at(currentIdx);
    currentIdx += 1;
    removeCount -= 1;
    return commands.at(currentIdx);
}
