import QtQuick

import Qt.BtcMiner

Flickable {
    id: root
    clip: true
    contentHeight: column.height
    property string memoryName
    property string memoryKey

    SystemStatsModule {
        id: stats_module
        memoryName: root.memoryName
        memoryKey: root.memoryKey
    }

    Column {
        id: column
        width: root.width
        Repeater {
            model: stats_module.model
            StatItem {
                required property var modelData
                required property int index
                height: 65
                width: 190
            }
        }
    }
}
