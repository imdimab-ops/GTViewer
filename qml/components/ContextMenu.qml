/* components/ContextMenu.qml - ContextMenu */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Menu {
    id: contextMenu

    property string name: ""
    property string symbol: ""
    property bool addToChart: false

    signal emitAddToChart(string symbol)
    signal emitShowPassport(string symbol)

    //Настройка отступов меню
    padding: 2

    //Заголовок
    MenuItem {
        text: symbol + " " + name
        font.pixelSize: 12
        font.bold: true
        padding: 5
        height: 28
    }

    MenuSeparator {
        padding: 0
    }

    MenuItem {
        text: "Добавить на график"
        icon.source: addToChart ? "img/chartOn.svg" : "img/chartOff.svg"
        icon.width: 20
        icon.height: 20
        font.pixelSize: 12
        padding: 5
        spacing: 10
        background: Rectangle {
            color: parent.highlighted ? "#E3F2FD" : "transparent"
            radius: 2
            anchors.fill: parent
            anchors.margins: 2
        }

        onTriggered: {
            emitAddToChart(symbol)
            contextMenu.close()
        }
    }

    MenuItem {
        text: "Паспорт параметра"
        icon.source: "img/passport.svg"
        icon.width: 20
        icon.height: 20
        font.pixelSize: 12
        padding: 5
        spacing: 10
        background: Rectangle {
            color: parent.highlighted ? "#E3F2FD" : "transparent"
            radius: 2
            anchors.fill: parent
            anchors.margins: 2
        }

        onTriggered: {
            emitShowPassport(symbol)
            contextMenu.close()
        }
    }
}
