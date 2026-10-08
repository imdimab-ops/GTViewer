/* components/PassportParam.qml */
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts

ApplicationWindow {
    id: passportWindow
    width: 400
    height: 560
    minimumWidth: 350
    minimumHeight: 400
    title: location + " Паспорт параметра: " + symbol
    modality: Qt.NonModal  //окна не блокируют друг друга
    flags: Qt.Dialog | Qt.WindowStaysOnTopHint | Qt.WindowCloseButtonHint | Qt.WindowTitleHint

    property string name: "Параметр"  // Название параметра
    property string symbol: "P1"      // Позиционное обозначение
    property real pv: 0.0             // (Process Variable) - текущее значение
    property real ah: 80.0            // Верхняя аварийная уставка
    property real ah2: 90.0           // Верхняя аварийная уставка 2
    property real al: 20.0            // Нижняя аварийная уставка
    property real al2: 10.0           // Нижняя аварийная уставка 2
    property real hyst: 0.0           // Гистерезис
    property real wh: 70.0            // Верхняя предупредительная уставка
    property real wl: 30.0            // Нижняя предупредительная уставка
    property real lrv: 0.0            // (Lower Range Value) – нижний предел шкалы измерения (0% шкалы)
    property real urv: 100.0          // (Upper Range Value) – верхний предел шкалы измерения (100% шкалы)
    property int bcw: 63              // Borders check word (Byte) 63 - 0011 1111
    property int stw: 0               // Word (0bit - rez, 1bit - Bad, 2bit - Warning, 3bit - Alarm)
    property bool sim: false          // Симуляция
    property string timestamp: ""
    property string unit: "un"        // Единицы измерения
    property string description: "descr"   // Описание
    property string tagname: "tag"    // Имя прочитанного тега (для запроса истории)
    property string location: "Location"

    //Функция обновления данных
    function updateData(newData) {
        if (newData) {
            name = newData.name !== undefined ? newData.name : name;
            symbol = newData.symbol !== undefined ? newData.symbol : symbol;
            pv = newData.pv !== undefined ? newData.pv : pv
            ah = newData.ah !== undefined ? newData.ah : ah
            ah2 = newData.ah2 !== undefined ? newData.ah2 : ah2
            al = newData.al !== undefined ? newData.al : al
            al2 = newData.al2 !== undefined ? newData.al2 : al2
            hyst = newData.hyst !== undefined ? newData.hyst : hyst
            wh = newData.wh !== undefined ? newData.wh : wh
            wl = newData.wl !== undefined ? newData.wl : wl
            lrv = newData.lrv !== undefined ? newData.lrv : lrv
            urv = newData.urv !== undefined ? newData.urv : urv
            bcw = newData.bcw !== undefined ? newData.bcw : bcw
            stw = newData.stw !== undefined ? newData.stw : stw
            sim = newData.sim !== undefined ? newData.sim : sim
            timestamp = newData.timestamp !== undefined ? newData.timestamp.toString() : timestamp;
            unit = newData.unit !== undefined ? newData.unit : unit;
            description = newData.description !== undefined ? newData.description : description;
            tagname = newData.tagname !== undefined ? newData.tagname : tagname;
            location = newData.location !== undefined ? newData.location : location;
        }
    }

    //Private (флаги проверки уставок)
    property bool _wh_check : (bcw & 0x01);     //bit0
    property bool _wl_check : (bcw & 0x02);     //bit1
    property bool _ah_check : (bcw & 0x04);     //bit2
    property bool _al_check : (bcw & 0x08);     //bit3
    property bool _ah2_check : (bcw & 0x10);    //bit4
    property bool _al2_check : (bcw & 0x20);    //bit5

    function _getStatusText() {
        if (stw & 0x02) return "Ошибка"
        if (stw & 0x08) return "Авария"
        if (stw & 0x04) return "Предупреждение"
        if (stw & 0x01) return "Норма"
        else return "Unknown"
    }
    function _getStatusColor() {
        if (stw & 0x02) return "#5B6262" //Ошибка
        if (stw & 0x08) return "#F44336" //Авария
        if (stw & 0x04) return "#FF9800" //Предупреждение
        if (stw & 0x01) return "#4CAF50" //Норма
        return "#5B6262"
    }

    //Основной контент
    ScrollView {
        anchors.fill: parent
        anchors.margins: 10
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: parent.width
            spacing: 5

            // Заголовок
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 60
                color: "#E3F2FD"
                radius: 8

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 15

                    Rectangle {
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                        color: sim ? "#BF88BF" : "#1976D2"
                        radius: 20

                        Text {
                            anchors.centerIn: parent
                            text: symbol.substring(0, 2)
                            font.pixelSize: 20
                            font.bold: true
                            color: "white"
                        }
                        SequentialAnimation on opacity {
                            running: sim  // Анимация работает только когда sim = true
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
                            color: sim ? "#BF88BF" : "#1976D2"
                        }

                        Text {
                            text: name
                            font.pixelSize: 14
                            color: "#333333"
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: 70
                        Layout.preferredHeight: 24
                        color: {
                            if (sim) return "#BF88BF"
                            return _getStatusColor()
                        }
                        radius: 12

                        Text {
                            anchors.centerIn: parent
                            text: sim ? "Симуляция" : _getStatusText().substring(0, 7) //сократили количество символов текста до 6
                            font.pixelSize: 11
                            font.bold: true
                            color: "white"
                        }
                    }
                }
            }

            //Основная информация
            GroupBox {
                Layout.fillWidth: true
                title: "Основные параметры"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    // Текущее значение
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
                                    text: "Текущее значение"
                                    font.pixelSize: 12
                                    color: "#666666"
                                }

                                RowLayout {
                                    spacing: 5

                                    Text {
                                        text: pv.toFixed(2)
                                        font.pixelSize: 28
                                        font.bold: true
                                        color: {
                                            if (stw & 0x02) return "#5B6262"
                                            if (stw & 0x08) return "#F44336"
                                            if (stw & 0x04) return "#FF9800"
                                            return "#1976D2"
                                        }
                                    }

                                    Text {
                                        text: unit
                                        font.pixelSize: 16
                                        color: "#666666"
                                    }
                                }
                            }

                            Item { Layout.fillWidth: true }

                            //Индикатор шкалы
                            ColumnLayout {
                                spacing: 2
                                Layout.alignment: Qt.AlignRight

                                Text {
                                    text: "Шкала"
                                    font.pixelSize: 10
                                    color: "#999999"
                                    Layout.alignment: Qt.AlignHCenter
                                    Layout.bottomMargin: (bcw !== 0) ? 10 : 0
                                }

                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 12
                                    radius: 6
                                    color: "#EEEEEE"

                                    Rectangle {
                                        width: Math.min(parent.width * (pv - lrv) / (urv - lrv), parent.width)
                                        height: parent.height
                                        radius: 6
                                        color: {
                                            if (stw & 0x02) return "#5B6262"
                                            if (stw & 0x08) return "#F44336"
                                            if (stw & 0x04) return "#FF9800"
                                            return "#4CAF50"
                                        }
                                        Behavior on width { NumberAnimation { duration: 300 } }
                                    }

                                    //Рисунок рисок для уставок (поверх индикатора)
                                    Item {
                                        anchors.fill: parent
                                        z: 2
                                        property var tickData: [
                                            { value: ah, color: "#F44336", label: "AH", check: _ah_check },
                                            { value: ah2, color: "#F44336", label: "AH2", check: _ah2_check },
                                            { value: al, color: "#F44336", label: "AL", check: _al_check },
                                            { value: al2, color: "#F44336", label: "AL2", check: _al2_check },
                                            { value: wh, color: "#FF9800", label: "WH", check: _wh_check },
                                            { value: wl, color: "#FF9800", label: "WL", check: _wl_check },
                                        ]
                                        Repeater {
                                            model: parent.tickData
                                            delegate: Item {
                                                x: parent.width * ((modelData.value - lrv) / (urv - lrv)) - 1
                                                y: -5
                                                width: 2
                                                height: parent.height
                                                visible: lrv <= modelData.value && modelData.value <= urv && modelData.check

                                                Rectangle {
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                    y: 1
                                                    width: 2
                                                    height: parent.height + 10
                                                    color: modelData.color
                                                    opacity: 0.9
                                                    border.color: modelData.color
                                                    border.width: 0.5
                                                }
                                                Text {
                                                    text: modelData.label
                                                    font.pixelSize: 10
                                                    font.bold: false
                                                    color: modelData.color
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                    y: parent.height - 25
                                                    opacity: 0.9
                                                    visible: modelData.label !== ""
                                                }
                                            }
                                        }
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.topMargin: (bcw !== 0) ? 2 : 0
                                    spacing: 0

                                    Text {
                                        text: lrv.toFixed(1)
                                        font.pixelSize: 9
                                        color: "#999999"
                                        Layout.fillWidth: true
                                    }

                                    Text {
                                        text: urv.toFixed(1)
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

            //Детальная информация
            GroupBox {
                Layout.fillWidth: true
                title: "Детальная информация"

                GridLayout {
                    anchors.fill: parent
                    columns: 5
                    rowSpacing: 5
                    columnSpacing: 15

                    //Строка 1
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
                    Rectangle {
                        width: 12
                        height: 12
                        radius: 2
                        border.color: "#999999"
                        border.width: 1.5
                        color: "transparent"
                        Rectangle {
                            visible: _ah2_check
                            width: 6
                            height: 6
                            radius: 3
                            x: (parent.width - width) / 2
                            y: (parent.height - height) / 2
                            color: "#2196F3"
                        }
                    }
                    Label {
                        text: "AH2:"
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: ah2.toFixed(2)
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignLeft
                    }

                    //Строка 2
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
                    Rectangle {
                        width: 12
                        height: 12
                        radius: 2
                        border.color: "#999999"
                        border.width: 1.5
                        color: "transparent"
                        Rectangle {
                            visible: _ah_check
                            width: 6
                            height: 6
                            radius: 3
                            x: (parent.width - width) / 2
                            y: (parent.height - height) / 2
                            color: "#2196F3"
                        }
                    }
                    Label {
                        text: "AH:"
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: ah.toFixed(2)
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignLeft
                    }

                    //Строка 3
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
                    Rectangle {
                        width: 12
                        height: 12
                        radius: 2
                        border.color: "#999999"
                        border.width: 1.5
                        color: "transparent"
                        Rectangle {
                            visible: _wh_check
                            width: 6
                            height: 6
                            radius: 3
                            x: (parent.width - width) / 2
                            y: (parent.height - height) / 2
                            color: "#2196F3"
                        }
                    }
                    Label {
                        text: "WH:"
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: wh.toFixed(2)
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignLeft
                    }

                    //Строка 4
                    Label {
                        text: "Верхний предел:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: urv.toFixed(2)
                        font.pixelSize: 12
                        color: "#333333"
                    }
                    Rectangle {
                        width: 12
                        height: 12
                        radius: 2
                        border.color: "#999999"
                        border.width: 1.5
                        color: "transparent"
                        Rectangle {
                            visible: _wl_check
                            width: 6
                            height: 6
                            radius: 3
                            x: (parent.width - width) / 2
                            y: (parent.height - height) / 2
                            color: "#2196F3"
                        }
                    }
                    Label {
                        text: "WL:"
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: wl.toFixed(2)
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignLeft
                    }

                    //Строка 5
                    Label {
                        text: "Нижний предел:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: lrv.toFixed(2)
                        font.pixelSize: 12
                        color: "#333333"
                    }
                    Rectangle {
                        width: 12
                        height: 12
                        radius: 2
                        border.color: "#999999"
                        border.width: 1.5
                        color: "transparent"
                        Rectangle {
                            visible: _al_check
                            width: 6
                            height: 6
                            radius: 3
                            x: (parent.width - width) / 2
                            y: (parent.height - height) / 2
                            color: "#2196F3"
                        }
                    }
                    Label {
                        text: "AL:"
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: al.toFixed(2)
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignLeft
                    }

                    //Строка 6
                    Label {
                        text: "Гистерезис:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: hyst.toFixed(2)
                        font.pixelSize: 12
                        color: "#333333"
                    }
                    Rectangle {
                        width: 12
                        height: 12
                        radius: 2
                        border.color: "#999999"
                        border.width: 1.5
                        color: "transparent"
                        Rectangle {
                            visible: _al2_check
                            width: 6
                            height: 6
                            radius: 3
                            x: (parent.width - width) / 2
                            y: (parent.height - height) / 2
                            color: "#2196F3"
                        }
                    }
                    Label {
                        text: "AL2:"
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: al2.toFixed(2)
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignLeft
                    }

                    //Строка 7
                    Label {
                        text: "Единицы измерения:"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#666666"
                        Layout.alignment: Qt.AlignRight
                    }
                    Label {
                        text: unit
                        font.pixelSize: 12
                        color: "#333333"
                    }
                    Item{}
                    Item{}
                    Item{}

                    //Строка 8
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
                    Item{}
                    Item{}
                    Item{}

                    //Строка 9
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
                        Layout.columnSpan: 4  // ← Занимает 4 колонки
                    }
                    Item{}
                    Item{}
                    Item{}
                }
            }

            //Описание (если есть)
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

            //Кнопка закрытия
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
