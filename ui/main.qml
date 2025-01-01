pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material

import Qt.BtcMiner
// Background:
//     Darkest Black: #121212
//     Deep Charcoal: #1E1E1E

// Primary Text:
//     Light Gray: #E0E0E0
//     Soft White: #D1D1D1

// Secondary Text:
//     Gray: #B0B0B0
//     Muted Gray: #7A7A7A

// Accent Colors:
//     Blue (Primary Accent): #4D8DFF
//     Cyan (Hover/Secondary Accent): #00B8D4
//     Green (Success/Action): #4CAF50
//     Red (Error/Warning): #F44336

// Borders/Dividers:
//     Dark Gray: #333333
//     Slightly Lighter Gray: #555555

// Highlights:
//     Soft Yellow (Highlight): #FFEB3B
//     Purple (Selection/Highlight): #9C27B0

ApplicationWindow {
    id: root
    width: Screen.width //* 0.5
    height: Screen.height //* 0.5
    visible: true
    Material.background: "#121212"

    BitcoinCoreDaemon {
        id: bitcoin_core_daemon
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottomMargin: 40
        anchors.leftMargin: 10
        anchors.topMargin: 10
        width: parent.width * 0.25
    }

    BitcoinMining {
        id: bitcoin_mining
        height: parent.height * 0.60
        anchors.bottom: bottom_panel.top
        anchors.left: bitcoin_core_daemon.right
        anchors.right: parent.right
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        anchors.rightMargin: 10
    }

    Controller {
        id: controller
        anchors.bottom: bitcoin_mining.top
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 10
        width: parent.width * 0.30
    }

    Rectangle {
        anchors.top: parent.top
        anchors.bottom: bitcoin_mining.top
        anchors.left: bitcoin_core_daemon.right
        anchors.right: controller.left
        anchors.margins: 10
        color: "transparent"
        border.width: 1
        border.color: "#333333"

        Rectangle {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: stats.left
            anchors.topMargin: 1
            anchors.bottomMargin: 1
            anchors.rightMargin: 1
            anchors.leftMargin: 1
            color: "transparent"
            clip: true

            MessageReceiverModule {
                id: date_time_receiver
                memoryName: "date_time"
                memoryKey: "SYSTEM_STATS"
                onMessageChanged: {
                    time.text = date_time_receiver.message.time
                    date.text = date_time_receiver.message.date
                }
            }

            MessageReceiverModule {
                id: loading_message
                memoryName: "status"
                memoryKey: "LOADING"
                onMessageChanged: {
                    loading_text.text = loading_message.message.message
                    if (loading_text.text.indexOf("Done") != -1) {
                        loading_text.text = ""
                        controller.updateButtonState()
                    }
                }
            }

            Text {
                id: time
                anchors.top: parent.top
                anchors.topMargin: 50
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#D1D1D1"
                font.pixelSize: 40
                font.bold: true
                font.letterSpacing: 2
                renderTypeQuality: Text.VeryHighRenderTypeQuality
            }

            Text {
                id: date
                anchors.top: time.bottom
                anchors.topMargin: 5
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#7A7A7A"
                font.pixelSize: 25
                renderTypeQuality: Text.VeryHighRenderTypeQuality
            }

            Text {
                id: loading_text
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#FFEB3B"
                font.pixelSize: 15
                renderTypeQuality: Text.VeryHighRenderTypeQuality
            }

        }


        Rectangle {
            id: stats
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            anchors.rightMargin: 5
            width: 400
            clip: true
            color: "transparent"

            StatView {
                id: cpu_stats
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                width: 200
                memoryName: "cpu"
                memoryKey: "SYSTEM_STATS"
            }

            StatView {
                id: memory_stats
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: cpu_stats.left
                anchors.rightMargin: 10
                width: 200
                memoryName: "other"
                memoryKey: "SYSTEM_STATS"
            }
        }


    }

    Pane {
        id: bottom_panel
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        height: 30
        Material.background: "#1E1E1E"
        Material.roundedScale: Material.NotRounded
    }

}
