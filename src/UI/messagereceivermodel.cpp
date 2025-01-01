//
// Created by aashik on 29/12/24.
//

#include <UI/messagereceivermodel.h>

MessageReceiverModel::MessageReceiverModel(QObject *parent) : QAbstractListModel(parent) {

    connect(this, &MessageReceiverModel::memoryNameChanged, this, [&]() {
        if (memoryNameFlag)
            throw std::runtime_error("Memory NAME is not changeable.");
        memoryNameFlag = true;
        if (!memoryKey.isEmpty())
            InitMessageReceiver();
    });
    connect(this, &MessageReceiverModel::memoryKeyChanged, this, [&]() {
        if (memoryKeyFlag)
            throw std::runtime_error("Memory KEY is not changeable.");
        memoryKeyFlag = true;
        if (!memoryName.isEmpty())
            InitMessageReceiver();
    });
}

int MessageReceiverModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_logs.count();
}

QVariant MessageReceiverModel::data(const QModelIndex &index, const int role) const
{
    if (!index.isValid())
        return {};

    if (role == MessageRole)
        return m_logs.at(index.row());

    return {};
}

QHash<int, QByteArray> MessageReceiverModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MessageRole] = "logMessage";
    return roles;
}

void MessageReceiverModel::addLogMessage(const QString &message)
{
    if (m_logs.count() >= MaxLogCount) {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_logs.removeAt(0);
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), m_logs.count(), m_logs.count());
    m_logs.append(message);
    endInsertRows();
    emit added();
}

void MessageReceiverModel::clearLogs()
{
    beginResetModel();
    m_logs.clear();
    endResetModel();
}

void MessageReceiverModel::InitMessageReceiver() {
    receiver = std::make_unique<Messenger::Receiver>(memoryName.toStdString(), memoryKey.toStdString());
    receiver->getMessage([&](const std::string& message) {
        addLogMessage(QString::fromStdString(message));
    });
}
