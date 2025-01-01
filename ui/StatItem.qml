import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material


Rectangle {
    id: root
    color: "transparent"
    Material.theme: Material.Dark

    Rectangle {
        id: progress
        width: 4
        radius: width
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        color: root.interpolateColor(root.modelData.stat)

        Rectangle {
            width: 2
            radius: width
            height: (parent.height-4) * (100-root.modelData.stat)/100
            anchors.top: parent.top
            anchors.topMargin: 2
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#121212"
        }
    }

    FontLoader {
        id: material_icons
        source: "qrc:/MaterialIcons-Regular.ttf"
    }

    Column {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: progress.right
        anchors.leftMargin: 10
        spacing: 5
        Text {
            text: root.modelData.name
            color: "#7A7A7A"
            font.pixelSize: 16
        }
        Text {
            text: `${root.modelData.stat.toFixed(2)}%`
            color: "#D1D1D1"
            font.bold: true
            font.pixelSize: 15
        }
    }


    Button {
        font.family: material_icons.name
        font.pointSize: 30
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        Material.roundedScale: Material.LargeScale
        text: "\uf108"
        height: 40
        width: 55
        flat: true
        highlighted: true
        Material.accent: root.interpolateColor(root.modelData.stat)
    }

    function interpolateColor(percentage) {
        if (percentage <= 0) return "#c2ff00";
        if (percentage >= 100) return "#ff0000";

        let color1, color2, rangeStart, rangeEnd;

        if (percentage <= 50) {
            color1 = Qt.rgba(194 / 255, 255 / 255, 0 / 255, 1);
            color2 = Qt.rgba(248 / 255, 255 / 255, 0 / 255, 1);
            rangeStart = 0;
            rangeEnd = 50;
        } else {
            color1 = Qt.rgba(248 / 255, 255 / 255, 0 / 255, 1);
            color2 = Qt.rgba(255 / 255, 0 / 255, 0 / 255, 1);
            rangeStart = 50;
            rangeEnd = 100;
        }

        let t = (percentage - rangeStart) / (rangeEnd - rangeStart);
        let r = color1.r + t * (color2.r - color1.r);
        let g = color1.g + t * (color2.g - color1.g);
        let b = color1.b + t * (color2.b - color1.b);

        return Qt.rgba(r, g, b, 1);
    }
}

