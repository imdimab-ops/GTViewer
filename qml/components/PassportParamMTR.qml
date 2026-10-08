/* components/PassportParamMTR.qml Паспорт параметра исполнительного механизма (MTR) */
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import "./paramLogicMTR.js" as Logic

ApplicationWindow {
    id: passportWindow
    width: 500
    height: 770
    minimumWidth: 360
    minimumHeight: 480
    title: location + " Паспорт механизма: " + symbol
    modality: Qt.NonModal
    flags: Qt.Dialog | Qt.WindowStaysOnTopHint | Qt.WindowCloseButtonHint | Qt.WindowTitleHint

    // ==== Public свойства (по аналогии с аналоговым каналом) ====
    property string name: "Motor"           // Название
    property string symbol: "M1"            // Позиционное обозначение
    property bool running: false            // Работа
    property bool block: false              // Блокировка
    property bool fault: false              // Неисправность
    property int mode: 0                    // Фактический режим управления (enum PsTechEE)
    property int state: 0                   // Состояние (enum PsTechEE)
    property int ctlw: 0                    // byte сигналы управления на ВУ см.PsTechEE
    property int diagnw: 0                  // word диагностики неисправностей
    property real blockw: 0                 // dword признаки запретов
    property real load: 0.0              // Токовая загрузка
    property real nominalLoad: 100.0     // Номинальная загрузка
    property real worktime: 0            //ч
    property string unit: "%"               // Единицы измерения
    property string description: "descr"
    property string tagname: "tag"
    property string location: "Location"
    property string timestamp: ""

    // ==== Функция обновления данных (аналог updateData) ====
    function updateData(newData) {
        if (newData) {
            name = newData.name !== undefined ? newData.name : name;
            symbol = newData.symbol !== undefined ? newData.symbol : symbol;
            running = newData.running !== undefined ? newData.running : running;
            block = newData.block !== undefined ? newData.block : block;
            fault = newData.fault !== undefined ? newData.fault : fault;
            mode = newData.mode !== undefined ? newData.mode : mode;
            state = newData.state !== undefined ? newData.state : state;
            ctlw = newData.ctlw !== undefined ? newData.ctlw : ctlw;
            diagnw = newData.diagnw !== undefined ? newData.diagnw : diagnw;
            blockw = newData.blockw !== undefined ? newData.blockw : blockw;
            load = newData.load !== undefined ? newData.load : load;
            worktime = newData.worktime !== undefined ? newData.worktime : worktime;
            unit = newData.unit !== undefined ? newData.unit : unit;
            description = newData.description !== undefined ? newData.description : description;
            tagname = newData.tagname !== undefined ? newData.tagname : tagname;
            location = newData.location !== undefined ? newData.location : location;
            timestamp = newData.timestamp !== undefined ? newData.timestamp.toString() : timestamp;
        }
    }

    // ==== Private: статус и цвета (логика из PumpLite) ====
    readonly property color _activeColor: "#3ab842"
    readonly property color _idleColor:   "#ffA000"
    readonly property color _faultColor:  "#F44336"
    readonly property color _blockColor:  "#9E9E9E"
    readonly property color _normColor:   "#4CAF50"

    function _getStatusText() {
        if (fault) return "Авария"
        if (block) return "Блокировка"
        if (running) return "Работа"
        return "Останов"
    }
    function _getStatusColor() {
        if (fault) return _faultColor
        if (block) return _blockColor
        if (running) return _activeColor
        return _idleColor
    }
    function _root_isRunning() { return running && !block && !fault }


    // ==== Основной контент ====
    ScrollView {
        anchors.fill: parent
        anchors.margins: 10
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: parent.width
            spacing: 5

            // ---------- Заголовок ----------
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 60
                color: "#E3F2FD"
                radius: 8

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 15

                    // Аватар (круг с типом/символом)
                    Rectangle {
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                        color: _root_isRunning() ? "#1976D2" : "#9E9E9E"
                        radius: 20

                        Text {
                            anchors.centerIn: parent
                            text: symbol.substring(0, 2)
                            font.pixelSize: 18
                            font.bold: true
                            color: "white"
                        }

                        // Пульсация при аварии
                        SequentialAnimation on opacity {
                            running: passportWindow.fault
                            loops: Animation.Infinite
                            PropertyAnimation { from: 1.0; to: 0.1; duration: 600 }
                            PropertyAnimation { from: 0.1; to: 1.0; duration: 600 }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Text {
                            text: symbol
                            font.pixelSize: 18
                            font.bold: true
                            color: _getStatusColor()
                        }

                        Text {
                            text: name
                            font.pixelSize: 14
                            color: "#333333"
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }

                    // Бейдж статуса
                    Rectangle {
                        Layout.preferredWidth: 80
                        Layout.preferredHeight: 24
                        color: _getStatusColor()
                        radius: 12

                        Text {
                            anchors.centerIn: parent
                            text: _getStatusText().substring(0, 9)
                            font.pixelSize: 11
                            font.bold: true
                            color: "white"
                        }
                    }
                }
            }

            // ---------- Основные параметры ----------
            GroupBox {
                Layout.fillWidth: true
                title: "Основные параметры"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    // Токовая загрузка + шкала
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 70
                        color: "#F5F5F5"
                        radius: 6

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 10

                            ColumnLayout {
                                spacing: 2

                                Text {
                                    text: "Загрузка"
                                    font.pixelSize: 12
                                    color: "#666666"
                                }

                                RowLayout {
                                    spacing: 5

                                    Text {
                                        text: load.toFixed(1)
                                        font.pixelSize: 28
                                        font.bold: true
                                        color: _getStatusColor()
                                    }

                                    Text {
                                        text: unit
                                        font.pixelSize: 16
                                        color: "#666666"
                                    }
                                }
                            }

                            Item { Layout.fillWidth: true }

                            // Индикатор шкалы загрузки
                            ColumnLayout {
                                spacing: 2
                                Layout.alignment: Qt.AlignRight

                                Text {
                                    text: "Шкала"
                                    font.pixelSize: 10
                                    color: "#999999"
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 12
                                    radius: 6
                                    color: "#EEEEEE"

                                    Rectangle {
                                        width: nominalLoad > 0
                                               ? Math.max(0, Math.min(parent.width * (load / nominalLoad), parent.width))
                                               : 0
                                        height: parent.height
                                        radius: 6
                                        color: _getStatusColor()
                                        Behavior on width { NumberAnimation { duration: 300 } }
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.topMargin: 2
                                    spacing: 0

                                    Text {
                                        text: "0"
                                        font.pixelSize: 9
                                        color: "#999999"
                                        Layout.fillWidth: true
                                    }

                                    Text {
                                        text: nominalLoad.toFixed(0)
                                        font.pixelSize: 9
                                        color: "#999999"
                                        Layout.alignment: Qt.AlignRight
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // ---------- Детальная информация ----------
            GroupBox {
                Layout.fillWidth: true
                title: "Детальная информация"

                GridLayout {
                    anchors.fill: parent
                    columns: 3
                    rowSpacing: 5
                    columnSpacing: 15

                    // Строка 1
                    Label {
                        text: "Тег:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: tagname
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                    Item {}

                    // Строка 2
                    Label {
                        text: "Обозначение:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: symbol
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                    Item {}

                    // Строка 3
                    Label {
                        text: "Название:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: name
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                    Item {}

                    // Строка 4 — режим управления
                    Label {
                        text: "Режим управления:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    RowLayout {
                        spacing: 6
                        Rectangle {
                            width: 14; height: 14
                            radius: 3
                            color: Logic.getModeColor(passportWindow)
                            Text {
                                anchors.centerIn: parent
                                text: Logic.getMode(passportWindow)
                                font.pixelSize: 9
                                font.bold: true
                                color: "white"
                            }
                        }
                        Label {
                            text: Logic.getModeText(passportWindow)
                            font.pixelSize: 12
                            color: "#333333"
                        }
                    }
                    Item {}

                    // Строка 5 — состояние
                    Label {
                        text: "Состояние:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: Logic.getStateText(passportWindow)
                        font.pixelSize: 12
                        color: _getStatusColor()
                        font.bold: true
                    }
                    Item {}

                    // Строка 6 — признаки
                    Label {
                        text: "Признаки:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    RowLayout {
                        spacing: 10

                        // Работа
                        RowLayout {
                            spacing: 4
                            Rectangle {
                                width: 12; height: 12
                                radius: 2
                                border.color: "#999999"
                                border.width: 1.5
                                color: "transparent"
                                Rectangle {
                                    visible: running
                                    width: 6; height: 6
                                    radius: 3
                                    anchors.centerIn: parent
                                    color: _activeColor
                                }
                            }
                            Label { text: "Работа"; font.pixelSize: 11; color: "#333333" }
                        }

                        // Блокировка
                        RowLayout {
                            spacing: 4
                            Rectangle {
                                width: 12; height: 12
                                radius: 2
                                border.color: "#999999"
                                border.width: 1.5
                                color: "transparent"
                                Rectangle {
                                    visible: block
                                    width: 6; height: 6
                                    radius: 3
                                    anchors.centerIn: parent
                                    color: _blockColor
                                }
                            }
                            Label { text: "Блок."; font.pixelSize: 11; color: "#333333" }
                        }

                        // Авария
                        RowLayout {
                            spacing: 4
                            Rectangle {
                                width: 12; height: 12
                                radius: 2
                                border.color: "#999999"
                                border.width: 1.5
                                color: "transparent"
                                Rectangle {
                                    visible: fault
                                    width: 6; height: 6
                                    radius: 3
                                    anchors.centerIn: parent
                                    color: _faultColor
                                }
                            }
                            Label { text: "Авария"; font.pixelSize: 11; color: "#333333" }
                        }
                    }
                    Item {}

                    // Строка 7 — ctlw
                    Label {
                        text: "Сигналы управления:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Frame {
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                        padding: 8

                        background: Rectangle {
                            color: "#FAFAFA"
                            border.color: "#CCCCCC"
                            border.width: 1
                            radius: 6
                        }
                        ColumnLayout {
                            Label {
                                text: "dec: " + ctlw
                                font.pixelSize: 12
                                font.family: "monospace"
                                color: "#333333"
                            }
                            Label {
                                text: "bits: " +  Logic.wordsToBitsToString(ctlw, 8)
                                font.pixelSize: 12
                                font.family: "monospace"
                                color: "#333333"
                            }
                            Label {
                                text: Logic.getCtrlwText(passportWindow)
                                font.pixelSize: 12
                                font.family: "monospace"
                                color: "#333333"
                            }
                        }
                    }

                    // Строка 8 — diagnw
                    Label {
                        text: "Диагностика:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Frame {
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                        padding: 8

                        background: Rectangle {
                            color: "#FAFAFA"
                            border.color: "#CCCCCC"
                            border.width: 1
                            radius: 6
                        }
                        ColumnLayout {
                            Label {
                                text: "dec: " + diagnw
                                font.pixelSize: 12
                                font.family: "monospace"
                                color: "#333333"
                            }
                            Label {
                                text: "bits: " +  Logic.wordsToBitsToString(diagnw)
                                font.pixelSize: 12
                                font.family: "monospace"
                                color: "#333333"
                            }
                            Label {
                                text: Logic.getDiagnText(passportWindow)
                                font.pixelSize: 12
                                font.family: "monospace"
                                color: "#333333"
                            }
                        }
                    }

                    // Строка 9 — blockw
                    Label {
                        text: "Запреты:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Frame {
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                        padding: 8

                        background: Rectangle {
                            color: "#FAFAFA"
                            border.color: "#CCCCCC"
                            border.width: 1
                            radius: 6
                        }
                        ColumnLayout {
                            Label {
                                text: "dec: " + blockw
                                font.pixelSize: 12
                                font.family: "monospace"
                                color: "#333333"
                            }
                            Label {
                                text: "bits: " +  Logic.wordsToBitsToString(blockw, 32)
                                font.pixelSize: 12
                                font.family: "monospace"
                                color: "#333333"
                            }
                            Label {
                                text: Logic.getBlockText(passportWindow)
                                font.pixelSize: 12
                                font.family: "monospace"
                                color: "#333333"
                            }
                        }
                    }

                    // Строка 10 — единицы
                    Label {
                        text: "Единицы измерения:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    RowLayout {
                        Layout.alignment: Qt.AlignLeft
                        Label {
                            text: unit
                            font.pixelSize: 12
                            color: "#333333"
                        }
                        Item { width: 40}
                        Label {
                            text: "Наработка, ч:"
                            font.pixelSize: 12
                            font.bold: true
                            color: "#666666"
                        }
                        Label {
                            text: worktime
                            font.pixelSize: 12
                            color: "#333333"
                        }
                    }
                    Item {}

                    // Строка 11 — статус
                    Label {
                        text: "Статус:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: _getStatusText()
                        font.pixelSize: 12
                        font.bold: true
                        color: _getStatusColor()
                    }
                    Item {}

                    // Строка 12 — обновлено
                    Label {
                        text: "Обновлено:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: timestamp
                        font.pixelSize: 12
                        color: "#999999"
                        font.italic: true
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        Layout.columnSpan: 2
                    }
                    Item {}
                }
            }

            // ---------- Описание ----------
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: description ? 60 : 0
                color: "#F9F9F9"
                radius: 4
                visible: description !== ""

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 2

                    Text {
                        text: "Описание:"
                        font.pixelSize: 11
                        font.bold: true
                        color: "#666666"
                    }

                    Text {
                        text: description
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }
                }
            }

            // ---------- Кнопка закрытия ----------
            Button {
                text: "Закрыть"
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 80
                Layout.preferredHeight: 30
                Layout.topMargin: 5
                Layout.bottomMargin: 5
                onClicked: passportWindow.close()
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
