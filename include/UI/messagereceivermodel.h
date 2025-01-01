//
// Created by aashik on 29/12/24.
//

#pragma once

#include <QAbstractListModel>
#include <Messenger/receiver.h>

class MessageReceiverModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(QString memoryName MEMBER memoryName NOTIFY memoryNameChanged)
    Q_PROPERTY(QString memoryKey MEMBER memoryKey NOTIFY memoryKeyChanged)

public:
    enum LogRoles {
        MessageRole = Qt::UserRole + 1
    };

    explicit MessageReceiverModel(QObject *parent = nullptr);

    Q_INVOKABLE void addLogMessage(const QString &message);

    Q_INVOKABLE void clearLogs();

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

signals:
    void memoryNameChanged();
    void memoryKeyChanged();
    void added();

private:
    void InitMessageReceiver();

    std::unique_ptr<Messenger::Receiver> receiver;
    QString memoryKey, memoryName;
    bool memoryKeyFlag = false, memoryNameFlag = false;
    QStringList m_logs;
    static const int MaxLogCount = 500;
};
