import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import Qt.BtcMiner

Rectangle {
    color: "transparent"
    border.width: 1
    border.color: "#333333"
    clip: true


    ControllerModule {
        id: controller_module
    }

    BitcoinCoreModule {
        id: bitcoin_core_module
    }

    FontLoader {
        id: material_icons
        source: "qrc:/MaterialIcons-Regular.ttf"
    }

    Pane {
        id: controller_panel
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: 1
        anchors.bottomMargin: 1
        anchors.rightMargin: 1
        width: 70
        clip: true
        Material.theme: Material.Dark
        Material.background: "#1E1E1E"
        Material.roundedScale: Material.NotRounded

        Column {
            Repeater {
                model: controller_module.model
                Button {
                    required property var modelData
                    required property int index
                    font.family: material_icons.name
                    font.pixelSize: 22
                    text: modelData.icon
                    height: 50
                    width: 50
                    flat: true
                    enabled: modelData.enabled
                    onClicked: controller_module.onButtonClicked(index)
                    Material.roundedScale: Material.SmallScale
                    Material.accent: enabled ? modelData.color : "transparent"
                    highlighted: true
                }
            }
        }
    }

    Logger {
        id: logger
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: controller_panel.left
        anchors.bottom: input_box.top
        anchors.topMargin: 10
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        memoryName: "command_response"
        memoryKey: "BTC_CLI"
    }

    Rectangle {
        id: input_box
        color: "transparent"
        border.color: "#333333"
        border.width: 1
        anchors.right: controller_panel.left
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        height: 30
        clip: true

        TextInput {
            color: "#D1D1D1"
            selectionColor: "#FFEB3B"
            selectedTextColor: "#121212"
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            font.pixelSize: 14
            persistentSelection: true
            onAccepted: {
                if (text === "clear")
                    logger.clear()
                bitcoin_core_module.runCommand(text)
                clear()
            }
            Keys.onPressed: (event) => {
                if (event.key == Qt.Key_Up)
                    text = bitcoin_core_module.decreamentCommand()
                if (event.key == Qt.Key_Down)
                    text = bitcoin_core_module.increamentCommand()
                if (event.key == Qt.Key_Home)
                    logger.moveUp()
                if (event.key == Qt.Key_End)
                    logger.moveDown()
                if (event.key == Qt.Key_Home && (event.modifiers & Qt.ControlModifier))
                    logger.fullUp()
                if (event.key == Qt.Key_End && (event.modifiers & Qt.ControlModifier))
                    logger.fullDown()
            }
        }
    }

    function updateButtonState() {
        controller_module.updateButtonState()
    }

}
