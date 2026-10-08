/* AI.qml - Analog Input standart */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "./paramLogicAI.js" as Logic

Rectangle {
    id: root
    width: 150
    height: 60
    color: "#e8e8e8"
    radius: 5
    border.color: _defaultBorderColor
    opacity: 0.9

    //Public свойства для настройки
    property string type: "AI"
    property string name: "Параметр"  // Название параметра
    property string symbol: "P1"      // Позиционное обозначение
    property real pv: 0.0             // (Process Variable) - текущее значение
    property real ah: 0.0             // Верхняя аварийная уставка
    property real ah2: 00.0           // Верхняя аварийная уставка 2
    property real al: 0.0             // Нижняя аварийная уставка
    property real al2: 0.0            // Нижняя аварийная уставка 2
    property real hyst: 0.0           // Гистерезис
    property real wh: 0.0             // Верхняя предупредительная уставка
    property real wl: 0.0             // Нижняя предупредительная уставка
    property real lrv: 0.0            // (Lower Range Value) – нижний предел шкалы измерения (0% шкалы)
    property real urv: 100.0          // (Upper Range Value) – верхний предел шкалы измерения (100% шкалы)
    property int bcw: 0               // Borders check word (Byte) 63 - 0011 1111
    property int stw: 0               // Word (0bit - rez, 1bit - Bad, 2bit - Warning, 3bit - Alarm)
    property bool sim: false          // Симуляция
    property string timestamp: ""
    property string unit: "un"        // Единицы измерения
    property string description: "descr"   // Описание
    property string tagname: "tag"    // Имя прочитанного тега (для запроса истории)
    property bool addTag: false       // True - тег добавлен на график, false - нет
    signal emitAddToChart (var data)

    //Private

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
        anchors {
            fill: parent
            margins: 1
        }
        spacing: 1

        //Верхняя строка: название и позиционное обозначение
        RowLayout {
            width: parent.width
            spacing: 5

            Text {
                text: root.symbol
                font.pixelSize: 14
                color: "#777777"
                Layout.alignment: Qt.AlignVCenter  //Выравнивание по вертикали
                Layout.preferredHeight: font.pixelSize + 2 //Фиксированная высота, немного больше font.pixelSize
            }

            Text {
                text: root.name
                font.bold: true
                font.pixelSize: 14
                color: "#555555"
                Layout.alignment: Qt.AlignVCenter  //Выравнивание по вертикали
                Layout.preferredHeight: font.pixelSize + 2 //Фиксированная высота, немного больше font.pixelSize
                Layout.fillWidth: true          // ← Занимает всё доступное место

                //Добавляем обрезание
                elide: Text.ElideRight          // ← Обрезать справа с "..."
                maximumLineCount: 1             // ← Одна строка
                wrapMode: Text.NoWrap           // ← Без переноса
            }

            Item {
                Layout.fillWidth: true
            }
        }

        //Основное значение
        RowLayout {
            width: parent.width
            Layout.topMargin: -5
            spacing: 2

            Text {
                text: (root.lrv - 50 <= root.pv && root.pv <= root.urv + 50 && (stw & 0x02) === 0) ? root.pv.toFixed(1) : "Bad"
                font.pixelSize: 30
                font.bold: true
                font.italic: sim ? true : false
                color: Logic.getStatusColor(root)
                opacity: 1.0
                Layout.fillWidth: true
                Layout.preferredHeight: font.pixelSize + 0 //Фиксированная высота, немного больше font.pixelSize
                horizontalAlignment: Text.AlignLeft  //Выравнивание по левому краю
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                text: root.unit.length > 4 ? root.unit.substring(0, 3) + '…' : root.unit
                font.bold: true
                font.pixelSize: 14
                color: "#555555"
                Layout.alignment: Qt.AlignVCenter  //Выравнивание по вертикали
                Layout.preferredHeight: font.pixelSize + 2 //Фиксированная высота, немного больше font.pixelSize
            }

        }

        //Прогресс-бар для визуализации
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 8
            radius: 5
            color: "#d3d3d3"
            Rectangle {
                id: progressBar
                width: Math.min(parent.width * (root.pv - root.lrv) / (root.urv - root.lrv), parent.width)
                height: parent.height
                radius: 5
                color: Logic.getProgressColor(root)
                Behavior on width { NumberAnimation { duration: 50 } }

                // Дополнительная пульсация для выделенных состояний
                SequentialAnimation {
                    running: Logic.getAlarm(root)
                    loops: Animation.Infinite

                    onStopped: {
                        //Когда анимация останавливается, сбрасываем opacity
                        progressBar.opacity = 1.0
                    }

                    ParallelAnimation {
                        NumberAnimation {
                            target: progressBar
                            property: "opacity"
                            from: 0.1; to: 1.0
                            duration: 750
                        }
                    }

                    ParallelAnimation {
                        NumberAnimation {
                            target: progressBar
                            property: "opacity"
                            from: 1.0; to: 0.1
                            duration: 750
                        }
                    }
                }
            }
        }
    }
}
