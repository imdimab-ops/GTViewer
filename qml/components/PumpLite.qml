/* PumpLite.qml - Pump unit (улитка + горизонтальные патрубки) см. библиотеку PsTechEE/MRT/Двигатель Regul */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "./paramLogicMTR.js" as Logic

Rectangle {
    id: root
    width: 85
    height: 65
    color: "#e8e8e8"
    radius: 5
    border.color: _defaultBorderColor
    opacity: 0.9

    // ==== Public свойства ====
    property string type: "MTR"
    property string name: "Насос"
    property string symbol: "H1"
    property bool running: false
    property bool block: false
    property bool fault: false
    property int mode: 25   //enum фактический режим управления см.PsTechEE
    property int state: 25  //enum состояние см.PsTechEE
    property int ctlw: 25   //byte сигналы управления на ВУ см.PsTechEE
    property int diagnw: 25   //word диагностика неисправностей см.PsTechEE
    property real blockw: 25  //dword признаки запретов см.PsTechEE (int в qml это 64х битные числа)
    property real load: 0.0
    property real nominalLoad: 100.0
    property real worktime: 25   //ч
    property string timestamp: ""
    property string unit: "%"
    property string description: "Насосный агрегат"
    property string tagname: "pump1"
    property bool addTag: false       // True - тег добавлен на график, false - нет

    signal emitAddToChart(var data)
    signal clicked()

    // ==== Private ====
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
        anchors { fill: parent; margins: 1 }
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
                text: root._statusText()
                font.pixelSize: 9
                font.bold: true
                color: root._statusColor()
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                elide: Text.ElideRight
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
        //  ЦЕНТРАЛЬНАЯ ОБЛАСТЬ — УЛИТКА + ДВА ГОРИЗОНТАЛЬНЫХ ПАТРУБКА
        // ============================================================
        Item {
            id: pumpArea
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            Layout.minimumHeight: 30
            Layout.maximumHeight: 30
            Layout.topMargin: 0
            Layout.bottomMargin: 0

            // ---------- ВСАСЫВАЮЩИЙ ПАТРУБОК (слева, горизонтальный) ----------
            Rectangle {
                id: suctionPipe
                x: -2
                y: pumpArea.height / 2 - 4
                width: volute.x - x + 4
                height: 8
                color: root._pipeColor
                border.color: root._bodyShadow
                border.width: 1
            }

            // Фланец всасывания (утолщение у корпуса)
            Rectangle {
                x: suctionPipe.x + suctionPipe.width - 4
                y: suctionPipe.y - 2
                width: 4
                height: suctionPipe.height + 4
                color: root._bodyShadow
                border.color: "#555"
                border.width: 0.5
            }

            // ---------- НАПОРНЫЙ ПАТРУБОК (справа, горизонтальный) ----------
            Rectangle {
                id: dischargePipe
                x: volute.x + volute.width - 3
                y: pumpArea.height / 2 - 4
                width: pumpArea.width - x + 2
                height: 8
                color: root._pipeColor
                border.color: root._bodyShadow
                border.width: 1
            }

            // Фланец напорный (утолщение у корпуса)
            Rectangle {
                x: dischargePipe.x
                y: dischargePipe.y - 2
                width: 4
                height: dischargePipe.height + 4
                color: root._bodyShadow
                border.color: "#555"
                border.width: 0.5
            }

            // ---------- КОРПУС НАСОСА (улитка) ----------
            Item {
                id: volute
                x: pumpArea.width / 2 - 20
                y: pumpArea.height / 2 - 20
                width: 40
                height: 40

                // Спиральный корпус (улитка): внешний круг
                Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: root._bodyColor
                    border.color: root._bodyShadow
                    border.width: 1.5
                }

                // Утолщение улитки (выступ) — переход к напорному патрубку
                Rectangle {
                    x: parent.width - 6
                    y: parent.height / 2 - 8
                    width: 16
                    height: 16
                    radius: 3
                    color: root._bodyColor
                    border.color: root._bodyShadow
                    border.width: 1.5
                }
                // Утолщение улитки (выступ) — переход к напорному патрубку
                // Внутренняя камера (тёмный круг)
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width - 12
                    height: parent.height - 12
                    radius: width / 2
                    color: "#696f76"
                }

                // Вращающееся рабочее колесо (крыльчатка)
                Item {
                    id: impeller
                    anchors.centerIn: parent
                    width: 18
                    height: 18

                    Repeater {
                        model: 6
                        Rectangle {
                            width: 12
                            height: 2
                            radius: 1
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
                        duration: 700
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

            // ---------- АНИМАЦИЯ ПОТОКА ----------
            Row {
                id: suctionFlow
                x: suctionPipe.x + 3
                y: suctionPipe.y + 2.5
                spacing: 5
                visible: _flowActive()

                Repeater {
                    model: Math.max(3, Math.floor((suctionPipe.width - 8) / 6))
                    Rectangle {
                        width: 2; height: 3
                        radius: 1
                        color: root._liquidColor
                        opacity: 0.85
                        SequentialAnimation on opacity {
                            running: suctionFlow.visible
                            loops: Animation.Infinite
                            NumberAnimation { from: 0.2; to: 1.0; duration: 400 }
                            NumberAnimation { from: 1.0; to: 0.2; duration: 400 }
                        }
                    }
                }
            }
            Row {
                id: dischargeFlow
                x: dischargePipe.x + 6
                y: dischargePipe.y + 2.5
                spacing: 4
                visible: _flowActive()

                Repeater {
                    model: Math.max(3, Math.floor((dischargePipe.width - 10) / 6))
                    Rectangle {
                        width: 2; height: 3
                        radius: 1
                        color: root._liquidColor
                        opacity: 0.9
                        SequentialAnimation on opacity {
                            running: dischargeFlow.visible
                            loops: Animation.Infinite
                            NumberAnimation { from: 0.2; to: 1.0; duration: 300 }
                            NumberAnimation { from: 1.0; to: 0.2; duration: 300 }
                        }
                    }
                }
            }

            // ---------- СТРЕЛКА ПОТОКА под правым (напорным) патрубком ----------
            Canvas {
                id: flowArrow
                x: dischargePipe.x + dischargePipe.width - 14
                y: dischargePipe.y + dischargePipe.height + 5
                width: 10; height: 8
                visible: _flowActive()
                opacity: 0.85
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    ctx.strokeStyle = root._liquidColor
                    ctx.fillStyle = root._liquidColor
                    ctx.lineWidth = 1.2
                    ctx.beginPath()
                    ctx.moveTo(1, 1)
                    ctx.lineTo(9, 4)
                    ctx.lineTo(1, 7)
                    ctx.stroke()
                    ctx.beginPath()
                    ctx.moveTo(0, 4)
                    ctx.lineTo(4, 2)
                    ctx.lineTo(4, 6)
                    ctx.closePath()
                    ctx.fill()
                }
            }
        }

        // ============================================================
        //  НИЖНЯЯ СТРОКА: ток + статус + прогресс
        // ============================================================
        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 0
            Layout.bottomMargin: 0
            spacing: 0
            // Прогресс-бар загрузки по току
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 3
                radius: 2
                color: "#d3d3d3"
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
        }
    }
}
