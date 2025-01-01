//
// Created by aashik on 01/01/25.
//

#pragma once

#include <QObject>
#include <BitCoinCore/bitcoincore.h>
#include <Messenger/sender.h>

class BitcoinCoreModule : public QObject {
    Q_OBJECT

public:
    explicit BitcoinCoreModule(QObject *parent = nullptr);

    Q_INVOKABLE void runCommand(const QString &);
    Q_INVOKABLE QString decreamentCommand();
    Q_INVOKABLE QString increamentCommand();

private:
    Callback _callback;
    std::unique_ptr<Messenger::Sender> messenger;
    QStringList commands;
    int removeCount = 0;
    int currentIdx = 0;
};
