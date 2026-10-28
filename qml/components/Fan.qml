/* Fan.qml - Fan unit (улитка + лопасти) см. библиотеку PsTechEE/MRT/Двигатель Regul */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "./paramLogicMTR.js" as Logic

Rectangle {
    id: root
    width: 150
    height: 75
    color: "#e8e8e8"
    radius: 5
    border.color: _defaultBorderColor
    opacity: 0.9

    // ==== Public свойства ====
    property string type: "MTR"
    property string name: "Вентилятор"
    property string symbol: "В1"
    property bool running: false
    property bool block: false
    property bool fault: false
    property int mode: 25   //enum фактический режим управления см.PsTechEE
    property int state: 25  //enum состояние см.PsTechEE
    property int ctlw: 25   //byte сигналы управления на ВУ см.PsTechEE
    property int diagnw: 25 //word диагностика неисправностей см.PsTechEE
    property real blockw: 25 //dword признаки запретов см.PsTechEE (int в qml это 64х битные числа)
    property string diagnd: "descr"
    property string blockd: "descr"
    property real load: 0.0
    property real nominalLoad: 100.0
    property real worktime: 25   //ч
    property string timestamp: ""
    property string unit: "%"
    property string description: "Вентилятор"
    property string tagname: "vent1"
    property double valrotation: 0    // угол поворота треугольника 0 - вправо, 90 - вниз, 180 - влево, 270 - вверх
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
        //  ВЕРХНЯЯ СТРОКА: symbol + name + режим
        // ============================================================
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 16
            Layout.minimumHeight: 16
            Layout.maximumHeight: 16
            Layout.topMargin: 0
            Layout.bottomMargin: 0
            spacing: 4

            Text {
                text: root.symbol
                font.pixelSize: 12
                color: "#777777"
                Layout.alignment: Qt.AlignVCenter
                Layout.preferredHeight: font.pixelSize + 2
            }

            Text {
                text: root.name
                font.bold: true
                font.pixelSize: 12
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
        //  ЦЕНТРАЛЬНАЯ ОБЛАСТЬ — Корпус
        // ============================================================
        Item {
            id: pumpArea
            width: 146
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            Layout.minimumHeight: 40
            Layout.maximumHeight: 40
            Layout.topMargin: 0
            Layout.bottomMargin: 0
            // Зеркалим все  "left".
            transform: Scale {
                origin.x: pumpArea.width / 2
                origin.y: pumpArea.height / 2
                xScale: 1
            }
            // ----- Поток всас -----
            Item {
                id: suctionFlow
                x: valrotation === 0 || valrotation === 180 ? pumpArea.width/2 - pumpArea.height - 30 : pumpArea.width/2 - pumpArea.height
                y: 0
                width: valrotation === 0 || valrotation === 180 ? pumpArea.height + 30 : pumpArea.height
                height: pumpArea.height
                clip: true
                visible: _flowActive()
                rotation: valrotation
                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 0  // вертикальный зазор между рядами
                    Repeater {
                        model:3
                        Row {
                            id: suctionFlowRow1
                            spacing: 2
                            width: suctionFlow.width
                            height: 10
                            Repeater {
                                model: Math.ceil((suctionFlow.width + 10) / 10) + 1
                                Canvas {
                                    rotation: 0
                                    width: 10
                                    height: suctionFlowRow1.height
                                    onPaint: {
                                        var ctx = getContext("2d")

                                        ctx.beginPath()
                                        ctx.moveTo(0, 3.5)
                                        ctx.lineTo(4.8, 3.5)
                                        ctx.lineTo(4.8, 0)
                                        ctx.lineTo(10, 5)
                                        ctx.lineTo(4.8, 10)
                                        ctx.lineTo(4.8, 6.75)
                                        ctx.lineTo(0, 6.75)
                                        ctx.closePath()

                                        ctx.fillStyle = root._liquidColor
                                        ctx.fill()

                                        ctx.strokeStyle = "black"
                                        ctx.lineWidth = 0
                                        ctx.stroke()
                                    }
                                }
                            }

                            // Напор быстрее всасывания — визуальный «разгон» на выходе
                            NumberAnimation on x {
                                running: _flowActive()
                                loops: Animation.Infinite
                                from: -12
                                to: 0
                                duration: 500
                            }
                        }
                    }
                }
            }
            // ----- Поток нагнетание -----
            Item {
                id: dischargeFlow
                x: pumpArea.width/2
                y: 0
                width: valrotation === 0 || valrotation === 180 ? pumpArea.height + 30 : pumpArea.height
                height: pumpArea.height
                clip: true
                visible: _flowActive()
                rotation: valrotation
                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 0  // вертикальный зазор между рядами
                    Repeater {
                        model:3
                        Row {
                            id: dischargeFlowRow1
                            spacing: 2
                            width: dischargeFlow.width
                            height: 10
                            Repeater {
                                model: Math.ceil((dischargeFlow.width + 10) / 10) + 1
                                Canvas {
                                    rotation: 0
                                    width: 10
                                    height: dischargeFlowRow1.height
                                    onPaint: {
                                        var ctx = getContext("2d")

                                        ctx.beginPath()
                                        ctx.moveTo(0, 3.5)
                                        ctx.lineTo(4.8, 3.5)
                                        ctx.lineTo(4.8, 0)
                                        ctx.lineTo(10, 5)
                                        ctx.lineTo(4.8, 10)
                                        ctx.lineTo(4.8, 6.75)
                                        ctx.lineTo(0, 6.75)
                                        ctx.closePath()

                                        ctx.fillStyle = root._liquidColor
                                        ctx.fill()

                                        ctx.strokeStyle = "black"
                                        ctx.lineWidth = 0
                                        ctx.stroke()
                                    }
                                }
                            }

                            // Напор быстрее всасывания — визуальный «разгон» на выходе
                            NumberAnimation on x {
                                running: _flowActive()
                                loops: Animation.Infinite
                                from: -12
                                to: 0
                                duration: 500
                            }
                        }
                    }
                }
            }
            // ---------- КОРПУС ВЕНТИЛЯТОРА ----------
            Item {
                id: volute
                x: pumpArea.width / 2 - 20
                y: pumpArea.height / 2 - 20
                width: 40
                height: 40

                // Корпус: внешний круг
                Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: root._bodyColor
                    border.color: root._bodyShadow
                    border.width: 1.5
                }

                // Внутренняя камера (тёмный круг)
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width - 12
                    height: parent.height - 12
                    radius: width / 2
                    color: "#696f76"
                }

                // Вращающееся рабочее колесо (лопасти)
                Item {
                    id: impeller
                    anchors.centerIn: parent
                    width: 18
                    height: 18

                    Repeater {
                        model: 6
                        Rectangle {
                            width: 14
                            height: 9
                            radius: 6
                            border.width: 1
                            color: root._statusColor()
                            x: impeller.width / 2
                            y: impeller.height / 2 - height / 2
                            transformOrigin: Item.Left
                            rotation: index * 60
                        }
                    }

                    Rectangle {
                        anchors.centerIn: parent
                        width: 6; height: 6
                        radius: 3
                        color: "#33373c"
                        border.color: "#1a1c1f"
                        border.width: 0.5
                    }

                    RotationAnimation on rotation {
                        running: root.running
                        loops: Animation.Infinite
                        from: 0; to: 360
                        duration: 2000
                    }
                }

                // Пульсация при аварии — красное кольцо вокруг корпуса
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: -2
                    radius: width / 2
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

        // ============================================================
        //  НИЖНЯЯ СТРОКА: ток + статус + прогресс
        // ============================================================
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 16
            Layout.minimumHeight: 16
            Layout.maximumHeight: 16
            Layout.topMargin: 0
            Layout.bottomMargin: 0
            spacing: 2

            Text {
                text: root.load.toFixed(1)
                font.pixelSize: 14
                font.bold: true
                font.italic: false
                color: root._statusColor()
                Layout.alignment: Qt.AlignVCenter
                opacity: exist_freq ? 1 : 0 // ← вместо visible, чтобы сохранить размер и статус не съезжал
            }

            Text {
                text: root.unit
                font.pixelSize: 10
                color: "#777777"
                Layout.alignment: Qt.AlignVCenter
                opacity: exist_freq ? 1 : 0 // ← вместо visible, чтобы сохранить размер и статус не съезжал
            }

            // Прогресс-бар загрузки по току
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 6
                radius: 3
                color: "#d3d3d3"
                opacity: exist_freq ? 1 : 0 // ← вместо visible, чтобы сохранить размер и статус не съезжал
                Rectangle {
                    id: progressBar
                    width: root.nominalLoad > 0
                           ? Math.max(0, Math.min(parent.width * (root.load / root.nominalLoad), parent.width))
                           : 0
                    height: parent.height
                    radius: 3
                    color: root._statusColor()
                    Behavior on width { NumberAnimation { duration: 60 } }
                }
            }

            // Статус
            Text {
                text: root._statusText()
                font.pixelSize: 9
                font.bold: true
                color: root._statusColor()
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }

}
