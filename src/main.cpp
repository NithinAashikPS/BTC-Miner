#include <iostream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQuickStyle>

#include <UI/messagereceivermodel.h>
#include <UI/bitcoincore_ui.h>
#include <UI/controller.h>
#include <UI/systemstats.h>
#include <UI/messagereceiver.h>

#include <System/systemstatlistener.h>

#include <BitCoinCore/bitcoincore.h>


int main(int argc, char *argv[]) {

    QQuickStyle::setStyle("Material");

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    const QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](const QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    qmlRegisterType<MessageReceiverModel>("Qt.BtcMiner", 1, 0, "MessageReceiverModel");
    qmlRegisterType<Controller>("Qt.BtcMiner", 1, 0, "ControllerModule");
    qmlRegisterType<SystemStats>("Qt.BtcMiner", 1, 0, "SystemStatsModule");
    qmlRegisterType<BitcoinCoreModule>("Qt.BtcMiner", 1, 0, "BitcoinCoreModule");
    qmlRegisterType<MessageReceiver>("Qt.BtcMiner", 1, 0, "MessageReceiverModule");

    engine.rootContext()->setContextProperty("title", APPLICATION_NAME);

    SystemStatListener::Init();
    BitCoinCore::Start();

    engine.load(url);

    // if (auto rootObjects = engine.rootObjects(); !rootObjects.isEmpty()) {
    //     if (const auto window = qobject_cast<QQuickWindow *>(rootObjects.first())) {
    //         window->setFlags(Qt::FramelessWindowHint | Qt::Window);
    //         window->showFullScreen();
    //     }
    // }

    return QGuiApplication::exec();
}
