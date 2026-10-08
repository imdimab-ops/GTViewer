/* AIpbv.qml - Analog Input progressBar vertical */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "./paramLogicAI.js" as Logic

Rectangle {
    id: root
    implicitWidth: 30
    implicitHeight: columnLayout.implicitHeight
    radius: 3
    opacity: 0.9
    // border.color: "#cccccc"
    color: "transparent"

    //Public свойства для настройки
    property string type: "AI"
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
    property bool addTag: false       // True - тег добавлен на график, false - нет
    signal emitAddToChart (var data)

    //Private (флаги проверки уставок)
    property bool _wh_check : (bcw & 0x01);     //bit0
    property bool _wl_check : (bcw & 0x02);     //bit1
    property bool _ah_check : (bcw & 0x04);     //bit2
    property bool _al_check : (bcw & 0x08);     //bit3
    property bool _ah2_check : (bcw & 0x10);    //bit4
    property bool _al2_check : (bcw & 0x20);    //bit5

    //Анимация для рамки
    property color _defaultBorderColor: "#cccccc"
    property color _hoverBorderColor: "#2196F3"
    Behavior on border.color {
        ColorAnimation {
            duration: 50
            easing.type: Easing.InOutQuad
        }
    }
    Behavior on border.width {
        NumberAnimation {
            duration: 50
            easing.type: Easing.InOutQuad
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.RightButton | Qt.LeftButton
        hoverEnabled: true  //отслеживание наведения
        onEntered: {
            //при наведении меняем цвет и ширину рамки
            root.border.color = _hoverBorderColor
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
                mouse.accepted = true
            }
        }
    }


    ColumnLayout {
        id: columnLayout
        anchors {
            fill: parent
            margins: 0
        }
        spacing: 1

        //Символ сверху
        Text {
            text: root.symbol
            font.pixelSize: root.symbol.length <= 5 ? 10 : 8
            color: "#777777"
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 2
        }

        //Прогресс-бар с текстом значения внутри
        Item {
            id: progressBarContainer
            Layout.fillWidth: true
            Layout.preferredHeight: 160
            Layout.preferredWidth: 20

            //Фон прогресс-бара
            Rectangle {
                id: progressBackground
                anchors.fill: parent
                radius: 3
                color: "#e0e0e0"

                //Заполненная часть прогресс-бара
                Rectangle {
                    id: progressBar
                    width: parent.width
                    radius: 3
                    anchors.bottom: parent.bottom
                    height: Math.max(2, Math.min(progressBackground.height * (root.pv - root.lrv) / (root.urv - root.lrv), progressBackground.height))
                    Behavior on height { NumberAnimation { duration: 50 } }

                    gradient: Gradient {
                        orientation: Gradient.Vertical
                        GradientStop {
                            id: grad1
                            position: 0.0
                            color: {
                                if (stw & 0x02) return "#5b6262"
                                if (stw & 0x08) return "#ff0000"
                                if (stw & 0x04) return "#ffA000"
                                return "#2ecc71"
                            }
                        }
                        GradientStop {
                            id: grad2
                            position: 0.5
                            color: {
                                if (stw & 0x02) return "#b4c2c2"
                                if (stw & 0x08) return "#e60000"
                                if (stw & 0x04) return "#ff8c00"
                                return "#29b765"
                            }
                        }
                        GradientStop {
                            id: grad3
                            position: 1.0
                            color: {
                                if (stw & 0x02) return "#a7b7b7"
                                if (stw & 0x08) return "#cc0000"
                                if (stw & 0x04) return "#ff7800"
                                return "#27ae60"
                            }
                        }
                    }

                    //Динамичная анимация перелива
                    SequentialAnimation {
                        running: true
                        loops: Animation.Infinite
                        ParallelAnimation {
                            NumberAnimation { target: grad1; property: "position"; from: 0.0; to: 1.0; duration: 5000; easing.type: Easing.InOutSine }
                            NumberAnimation { target: grad2; property: "position"; from: 0.33; to: 1.0; duration: 5000; easing.type: Easing.InOutSine }
                            NumberAnimation { target: grad3; property: "position"; from: 0.66; to: 1.0; duration: 5000; easing.type: Easing.InOutSine }
                        }
                        ParallelAnimation {
                            NumberAnimation { target: grad1; property: "position"; from: 1.0; to: 0.0; duration: 5000; easing.type: Easing.InOutSine }
                            NumberAnimation { target: grad2; property: "position"; from: 1.0; to: 0.33; duration: 5000; easing.type: Easing.InOutSine }
                            NumberAnimation { target: grad3; property: "position"; from: 1.0; to: 0.66; duration: 5000; easing.type: Easing.InOutSine }
                        }
                        PauseAnimation { duration: 500 } //Добавляем небольшую паузу между циклами
                    }

                    //Дополнительная пульсация для выделенных состояний
                    SequentialAnimation {
                        running: Logic.getAlarm(root)
                        loops: Animation.Infinite
                        onStopped: { progressBar.opacity = 1.0 } //Когда анимация останавливается, сбрасываем opacity
                        NumberAnimation { target: progressBar; property: "opacity"; from: 0.1; to: 1.0; duration: 750 }
                        NumberAnimation { target: progressBar; property: "opacity"; from: 1.0; to: 0.1; duration: 750 }
                    }
                }

                //рисунок рисок для уставок (аналогично паспорту)
                //рисунок рисок для уставок (аналогично паспорту)
                Item {
                    anchors.fill: parent
                    z: 2
                    property var warningData: [
                        { value: root.wh, color: "#FF9800", label: "", check: root._wh_check },
                        { value: root.wl, color: "#FF9800", label: "", check: root._wl_check },
                    ]
                    property var alarmData: [
                        { value: root.ah, color: "#F44336", label: "", check: root._ah_check },
                        { value: root.ah2, color: "#F44336", label: "", check: root._ah2_check },
                        { value: root.al, color: "#F44336", label: "", check: root._al_check },
                        { value: root.al2, color: "#F44336", label: "", check: root._al2_check },
                    ]
                    Repeater {
                        model: parent.warningData
                        delegate: Rectangle {
                            x: 0
                            width: parent.width * 0.4
                            y: parent.height * (1 - (modelData.value - root.lrv) / (root.urv - root.lrv)) - 1
                            height: 2
                            visible: root.lrv <= modelData.value && modelData.value <= root.urv && modelData.check
                            color: modelData.color
                            opacity: 0.9
                            border.color: modelData.color
                            border.width: 0.5
                        }
                    }
                    Repeater {
                        model: parent.alarmData
                        delegate: Rectangle {
                            x: parent.width * 0.6
                            width: parent.width * 0.4
                            y: parent.height * (1 - (modelData.value - root.lrv) / (root.urv - root.lrv)) - 1
                            height: 2
                            visible: root.lrv <= modelData.value && modelData.value <= root.urv && modelData.check
                            color: modelData.color
                            opacity: 0.9
                            border.color: modelData.color
                            border.width: 0.5
                        }
                    }
                }

                //Текст значения поверх прогресс-бара
                Text {
                    text: root.urv
                    font.pixelSize: 10
                    color: "#4f4f4f"
                    anchors { top: parent.top; topMargin: 2; horizontalCenter: parent.horizontalCenter }
                }
                Text {
                    text: (root.lrv - 50 <= root.pv && root.pv <= root.urv + 50 && (stw & 0x02) === 0) ? root.pv.toFixed(0) : "Bad"
                    font.pixelSize: 14
                    font.bold: true
                    font.italic: sim ? true : false
                    color: sim ? "#BF88BF" : "#4f4f4f"
                    anchors.centerIn: parent
                }
                Text {
                    text: root.lrv
                    font.pixelSize: 10
                    color: "#4f4f4f"
                    anchors { bottom: parent.bottom; bottomMargin: 2; horizontalCenter: parent.horizontalCenter }
                }
            }
        }

        //Контейнер для вертикального названия (внизу)
        Item {
            id: nameContainer
            Layout.preferredWidth: 20
            Layout.preferredHeight: textItem.implicitWidth
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 2
            Layout.bottomMargin: 2

            Text {
                id: textItem
                text: root.name
                font.bold: true
                font.pixelSize: 11
                color: "#555555"
                rotation: -90
                anchors.centerIn: parent
                elide: Text.ElideRight
                maximumLineCount: 1
            }
        }
    }
}
