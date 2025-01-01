import QtQuick
import QtQuick.Controls

import Qt.BtcMiner

Item {
    id: root
    property string memoryName
    property string memoryKey

    ListView {
        id: logTextArea
        anchors.fill: parent
        clip: true
        model: MessageReceiverModel {
            memoryName: root.memoryName
            memoryKey: root.memoryKey
            onAdded: {
                logTextArea.positionViewAtEnd()
                logTextArea.incrementCurrentIndex()
            }
        }
        delegate: TextArea {
            width: logTextArea.width
            text: root.applyDateStyle(model.logMessage)
            font.pixelSize: 15
            color: "#D1D1D1"
            textFormat: Text.RichText
            wrapMode: Text.WrapAnywhere
            selectionColor: "#FFEB3B"
            selectedTextColor: "#121212"
            clip: true
            readOnly: true
            background: null
        }
    }

    function applyDateStyle(message) {
        if (!message)
            return ""
        let regex = /(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z)/g;
        let styledMessage = message.replace(regex, function(match) {
            if (message.indexOf(match) === 0)
                return `<font color="#4D8DFF"> ${match}</font>`;
            return match; // For other matches, return as-is
        });
        if (message.startsWith("Error: ")) {
            styledMessage = `<font color="red">Error: </font>` + message.slice(7);
        }
        if (message.startsWith("Success: ")) {
            styledMessage = `<font color="#32cd32">Success: </font>` + message.slice(9);
        }
        if (message.startsWith("> ")) {
            styledMessage = `<br><font color="#4D8DFF">>>>&nbsp;</font>` + message.slice(2);
        }
        styledMessage = styledMessage
            .replace(/\n/g, "<br>")
            .replace(/\t/g, "&nbsp;&nbsp;&nbsp;&nbsp;");
        return styledMessage.trim(); // Trim the resulting string
    }

    function clear() {
        logTextArea.model.clearLogs()
    }

    function fullUp() {
        logTextArea.positionViewAtBeginning()
    }

    function fullDown() {
        logTextArea.positionViewAtEnd()
    }

    function moveUp() {
        logTextArea.positionViewAtIndex(logTextArea.currentIndex - 1, ListView.Contain)
        logTextArea.decrementCurrentIndex()
    }

    function moveDown() {
        logTextArea.positionViewAtIndex(logTextArea.currentIndex + 1, ListView.Contain)
        logTextArea.incrementCurrentIndex()
    }
}
