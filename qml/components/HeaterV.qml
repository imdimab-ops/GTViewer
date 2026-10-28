/* HeaterV.qml - HeaterV unit (вертикальный нагреватель) см. библиотеку PsTechEE/MRT/Двигатель Regul */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "./paramLogicMTR.js" as Logic

Rectangle {
    id: root
    width: 80
    height: 120
    color: "#e8e8e8"
    radius: 5
    border.color: _defaultBorderColor
    opacity: 0.9

    // ==== Public свойства ====
    property string type: "MTR"
    property string name: "Нагреватель"
    property string symbol: "H1"
    property bool running: true
    property bool block: false
    property bool fault: false
    property int mode: 25   //enum фактический режим управления см.PsTechEE
    property int state: 25  //enum состояние см.PsTechEE
    property int ctlw: 25   //byte сигналы управления на ВУ см.PsTechEE
    property int diagnw: 25 //word диагностика неисправностей см.PsTechEE
    property real blockw: 25 //dword признаки запретов см.PsTechEE (int в qml это 64х битные числа)
    property string diagnd: "descr"
    property string blockd: "descr"
    property real load: 20.0
    property real nominalLoad: 100.0
    property real worktime: 25   //ч
    property string timestamp: ""
    property string unit: "%"
    property string description: "Нагреватель"
    property string tagname: "heater1"
    property double valrotation: 0    // угол поворота зигзага 0 - вертикально, 90 - горизонтально и т.д.
    property bool exist_freq: true    // Наличие датчика тока/частоты true - есть, false - нету
    property bool addTag: false       // True - тег добавлен на график, false - нет

    signal emitAddToChart(var data)
    signal clicked()

    property color _defaultBorderColor: "#cccccc"
    property color _hoverBorderColor: "#2196F3"

    Behavior on border.color {
        ColorAnimation { duration: 50; easing.type: Easing.InOutQuad }
    }
    Behavior on border.width {
        NumberAnimation { duration: 50; easing.type: Easing.InOutQuad }
    }

    // ----- Цвета -----
    readonly property color _bodyColor: "#8a8f96"
    readonly property color _bodyShadow: "#6b7076"
    readonly property color _pipeColor: "#9aa0a6"
    readonly property color _liquidColor: "#2196F3"
    readonly property color _activeColor: "#3ab842"
    readonly property color _idleColor: "#ffA000"
    readonly property color _faultColor: "#ff0000"
    readonly property color _blockColor: "#9e9e9e"

    function _statusColor() {
        if (fault) return _faultColor
        if (block) return _blockColor
        if (running) return _activeColor
        return _idleColor
    }
    function _statusText() {
        if (fault) return "АВАРИЯ"
        if (block) return "БЛОК."
        if (running) return "РАБОТА"
        return "ОСТАНОВ"
    }
    function _flowActive() {
        return root.running && !root.block
    }

    // ----- Мышь -----
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.RightButton | Qt.LeftButton
        hoverEnabled: true
        onEntered: {
            root.border.color = root._hoverBorderColor
            root.border.width = 1
        }
        onExited: {
            root.border.color = _defaultBorderColor
            root.border.width = 0
        }
        onClicked: function(mouse) {
            if (mouse.button === Qt.RightButton) {
                Logic.showContextMenu(root)
                mouse.accepted = true
            } else if (mouse.button === Qt.LeftButton) {
                root.clicked()
                mouse.accepted = true
            }
        }
    }

    ColumnLayout {
        anchors { fill: parent; margins: 2 }
        spacing: 0

        // ============================================================
        //  ВЕРХНЯЯ СТРОКА: symbol + name + режим (сверху слева)
        // ============================================================
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 16
            Layout.minimumHeight: 16
            Layout.maximumHeight: 16
            Layout.topMargin: 0
            Layout.bottomMargin: 0
            spacing: 3

            Text {
                text: root.symbol
                font.pixelSize: 11
                color: "#777777"
                Layout.alignment: Qt.AlignVCenter
                Layout.preferredHeight: font.pixelSize + 2
            }

            Text {
                text: root.name
                font.bold: true
                font.pixelSize: 11
                color: "#555555"
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
                Layout.preferredHeight: font.pixelSize + 2
                elide: Text.ElideRight
                maximumLineCount: 1
                wrapMode: Text.NoWrap
            }

            // Индикатор местный/дистанционный
            Rectangle {
                width: 13; height: 13
                radius: 2
                color: Logic.getModeColor(root)
                Layout.alignment: Qt.AlignVCenter
                Text {
                    anchors.centerIn: parent
                    text: Logic.getMode(root)
                    font.pixelSize: 9
                    font.bold: true
                    color: "white"
                }
            }
        }

        // ============================================================
        //  ЦЕНТРАЛЬНАЯ ОБЛАСТЬ — корпус по центру + прогресс/ток справа
        // ============================================================
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // ---------- ВЕРТИКАЛЬНЫЙ КОРПУС (по центру, уменьшен) ----------
            RowLayout {
                id: centerRow
                anchors.centerIn: parent
                height: parent.height
                spacing: 1

                Item {
                    id: pumpArea
                    Layout.preferredWidth: 40
                    Layout.fillHeight: true
                    Layout.topMargin: 0
                    Layout.bottomMargin: 0

                    Item {
                        id: volute
                        x: 2
                        y: 2
                        width: pumpArea.width - 4
                        height: pumpArea.height - 4

                        // Корпус: внешний прямоугольник (вертикальный)
                        Rectangle {
                            anchors.fill: parent
                            color: root._bodyColor
                            border.color: root._bodyShadow
                            border.width: 1.5
                        }

                        // Внутренняя камера
                        Rectangle {
                            anchors.centerIn: parent
                            width: parent.width - 8
                            height: parent.height - 8
                            radius: width / 15
                            color: "#696f76"
                        }

                        // Зигзаг (вертикальный)
                        Item {
                            id: impeller
                            anchors.centerIn: parent
                            width: parent.width - 8
                            height: parent.height - 8
                            Canvas {
                                x: 4
                                y: 4
                                rotation: valrotation
                                width: parent.width - 8
                                height: parent.height - 8
                                onPaint: {
                                    var ctx = getContext("2d")
                                    ctx.beginPath()
                                    ctx.moveTo(width/2, height*(1/24))       // центр-верх
                                    ctx.lineTo(width/2, height*(3/24))
                                    ctx.lineTo(2, height*(4/24))              // влево
                                    ctx.lineTo(width - 2, height*(6/24))      // вправо
                                    ctx.lineTo(2, height*(8/24))              // влево
                                    ctx.lineTo(width - 2, height*(10/24))     // вправо
                                    ctx.lineTo(2, height*(12/24))             // влево
                                    ctx.lineTo(width - 2, height*(14/24))     // вправо
                                    ctx.lineTo(2, height*(16/24))             // влево
                                    ctx.lineTo(width - 2, height*(18/24))     // вправо
                                    ctx.lineTo(2, height*(20/24))             // влево
                                    ctx.lineTo(width/2, height*(21/24))       // центр-низ
                                    ctx.lineTo(width/2, height*(23/24))
                                    ctx.strokeStyle = root._statusColor()
                                    ctx.lineWidth = 2
                                    ctx.stroke()
                                }
                            }
                        }

                        // Пульсация при аварии
                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 2
                            radius: width / 15
                            color: "transparent"
                            border.color: root._faultColor
                            border.width: 2
                            visible: root.fault
                            SequentialAnimation on opacity {
                                running: root.fault
                                loops: Animation.Infinite
                                NumberAnimation { from: 0.1; to: 1.0; duration: 400 }
                                NumberAnimation { from: 1.0; to: 0.1; duration: 400 }
                            }
                        }
                    }
                }

                // ---------- ПРАВАЯ КОЛОНКА: вертикальный прогресс + [значение + единица] ----------
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: implicitWidth //Ширина = ширине самого широкого ребёнка
                    spacing: 2
                    visible: exist_freq

                    // Вертикальный прогресс-бар (заполнение снизу вверх)
                    Rectangle {
                        id: progressBg
                        Layout.fillHeight: true
                        Layout.preferredWidth: 10
                        Layout.alignment: Qt.AlignHCenter
                        radius: 3
                        color: "#d3d3d3"

                        Rectangle {
                            id: progressBar
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: root.nominalLoad > 0
                                    ? Math.max(0, Math.min(parent.height * (root.load / root.nominalLoad), parent.height))
                                    : 0
                            radius: 3
                            color: root._statusColor()
                            Behavior on height { NumberAnimation { duration: 60 } }
                        }
                    }

                    // Значение и единица под прогресс-баром
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 1

                        Text {
                            text: root.load.toFixed(1)
                            font.pixelSize: 10
                            font.bold: true
                            font.italic: false
                            color: root._statusColor()
                            Layout.alignment: Qt.AlignVCenter
                            elide: Text.ElideRight
                        }

                        Text {
                            text: root.unit
                            font.pixelSize: 9
                            color: "#777777"
                            Layout.alignment: Qt.AlignVCenter
                        }
                    }
                }
            }
        }

        // ============================================================
        //  НИЖНЯЯ СТРОКА: статус (снизу слева)
        // ============================================================
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 16
            Layout.minimumHeight: 16
            Layout.maximumHeight: 16
            Layout.topMargin: 0
            Layout.bottomMargin: 0
            spacing: 2

            Item { Layout.fillWidth: true }

            Text {
                text: root._statusText()
                font.pixelSize: 9
                font.bold: true
                color: root._statusColor()
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }
        }
    }
}
