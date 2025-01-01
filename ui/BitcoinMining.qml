import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material


Rectangle {
    color: "#121212"
    border.width: 1
    border.color: "#333333"

    Pane {
        id: title_panel
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 1
        anchors.rightMargin: 1
        anchors.leftMargin: 1
        height: 30
        Material.background: "#1E1E1E"
        Material.roundedScale: Material.NotRounded

        Text {
            text: "Bitcoin Mining"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            font.bold: true
            font.pointSize: 11
            color: "#B0B0B0"
            renderTypeQuality: Text.VeryHighRenderTypeQuality
        }
    }
}

