import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Shapes
import "./components"
import "./js/connectionsLogicMS.js" as Logic

//Перемещаем изображение после масштабирования
Item {
    id: root
    anchors.fill: parent

    //натсроили более приятный фон для глаз
    Rectangle {
        anchors.fill: parent
        color: "#f2f2f2"
        border.width: 0
        z: -10
    }

    //предпочтительный размер
    implicitWidth: 2048
    implicitHeight: 1080

    //используем implicit размеры для масштабирования
    property real _contentScale: Math.min(width / implicitWidth, height / implicitHeight)

    Component.onCompleted: {
        Logic.initializeContext(appEng)
        console.log("QML MS size:", width, height)
    }

    Connections {
        target: appEng
        function onFdataUpdateAI(arg) {
            Logic.handleDataUpdateAI(arg)
        }
    }
    Connections {
        target: appEng
        function onFdataUpdateMTR(arg) {
            Logic.handleDataUpdateMTR(arg)
        }
    }

    Item {
        id: content
        width: root.implicitWidth
        height: root.implicitHeight
        scale: root._contentScale
        anchors.centerIn: parent

        //Параллельная анимация для плавного масштабирования и перемещения
        ParallelAnimation {
            id: zoomAnimation
            NumberAnimation {
                target: imageScale
                properties: "xScale,yScale"
                duration: 200
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                target: root
                property: "imagePosition.x"
                duration: 200
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                target: root
                property: "imagePosition.y"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
        //Анимация возврата в исходное состояние
        ParallelAnimation {
            id: resetAnimation
            NumberAnimation {
                target: imageScale
                properties: "xScale,yScale"
                to: 1.0  //Возврат к исходному масштабу
                duration: 200
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                target: root
                property: "imagePosition.x"
                to: 0
                duration: 200
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                target: root
                property: "imagePosition.y"
                to: 0
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
        //Контейнер для изображения с трансформациями
        Item {
            id: imageContainer
            width: imgBlock1.implicitWidth
            height: imgBlock1.implicitHeight
            anchors.centerIn: parent

            transform: [
                Translate {
                    id: imageTranslate
                },
                Scale {
                    id: imageScale
                }
            ]

            Image {
                id: imgBlock1
                anchors.centerIn: parent
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/ms/base.svg"
                sourceSize: Qt.size(2048, 1080) //Указываем явный размер, иначе Qt уменьшает SVG до размера экрана при загрузке

                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.color: "#cccccc"
                    border.width: 0
                    radius: 1
                }
            }

            ColumnLayout {
                spacing: 5
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 460
                    leftMargin: 465
                }
                AI {
                    name: "Pм вх АВОМ"
                    symbol: "PT211"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                }
                AI {
                    name: "Tм вх АВОМ"
                    symbol: "TT201"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                }
            }

            ColumnLayout {
                spacing: 17
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 847
                    leftMargin: 1375
                }
                Pump {
                    name: "НМС1"
                    symbol: "M201"
                    flowDirection: "left"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 1.00
                    running: false
                    load: 0.0
                }
                Pump {
                    name: "НМС2"
                    symbol: "M202"
                    flowDirection: "left"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 1.00
                    running: false
                    load: 0.0
                }
            }
            AI {
                name: "Pм вых НМС"
                symbol: "PT203"
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 870
                    left: imgBlock1.left
                    leftMargin: 1090
                }
            }
            AI {
                name: "Перепад"
                symbol: "PDT204"
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 820
                    left: imgBlock1.left
                    leftMargin: 785
                }
            }

            AI {
                name: "T теплооб"
                symbol: "TT203"
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 190
                    left: imgBlock1.left
                    leftMargin: 620
                }
            }
            AI {
                name: "Tм вых АВОМ"
                symbol: "TT202"
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 530
                    left: imgBlock1.left
                    leftMargin: 715
                }
            }
            RowLayout {
                spacing: 5
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 575
                    leftMargin: 935
                }
                AI {
                    name: "Tм после АВОМ"
                    symbol: "TT205"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                }
                AI {
                    name: "Fм"
                    symbol: "FT201"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                }
            }
            ColumnLayout {
                spacing: 5
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 520
                    leftMargin: 1300
                }
                AI {
                    name: "Pм ГТД т.1"
                    symbol: "PT206A"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                }
                AI {
                    name: "Pм ГТД т.2"
                    symbol: "PT206B"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                }
            }
            RowLayout {
                spacing: 135
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 100
                    leftMargin: 1300
                }
                AI {
                    name: "Tм вых редук"
                    symbol: "TT206"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                }
                AI {
                    name: "Tм вых ГТД"
                    symbol: "TT207"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                }
            }
            ColumnLayout {
                spacing: 5
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 727
                    leftMargin: 1869
                }
                AI {
                    name: "Lм в баке"
                    symbol: "LT201"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                }
                AI {
                    name: "Tм в баке"
                    symbol: "TT204"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                }
            }
            ColumnLayout {
                width: 300
                height: 350
                spacing: 0
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 170
                    leftMargin: 160
                }
                RowLayout {
                    width: 600
                    height: 300
                    spacing: 10
                    Fan {
                        name: "ВЭКМО1"
                        symbol: "M1_AC1"
                        Component.onCompleted: Logic.registerComponent(this)
                        scale: 1
                        valrotation: 0
                    }
                    HeaterV {
                        type: 'MTR_nofc' //отключает контекст добавления на график
                        name: "ЭКМО1"
                        symbol: "AC1"
                        Component.onCompleted: Logic.registerComponent(this)
                        scale: 0.9
                        valrotation: 0
                        exist_freq: false
                    }
                }
                RowLayout {
                    width: 600
                    height: 300
                    spacing: 10
                    Fan {
                        name: "ВЭКМО2"
                        symbol: "M1_AC2"
                        Component.onCompleted: Logic.registerComponent(this)
                        scale: 1
                        valrotation: 0
                    }
                    HeaterV {
                        type: 'MTR_nofc' //отключает контекст добавления на график
                        name: "ЭКМО2"
                        symbol: "AC2"
                        Component.onCompleted: Logic.registerComponent(this)
                        scale: 1
                        valrotation: 0
                        exist_freq: false
                    }
                }

            }
            HeaterV {
                type: 'MTR_nofc' //отключает контекст добавления на график
                name: "EK205"
                symbol: "EK203_205"
                Component.onCompleted: Logic.registerComponent(this)
                scale: 1
                valrotation: 0
                exist_freq: false
                anchors {
                    top: imgBlock1.top
                    topMargin: 610
                    left: imgBlock1.left
                    leftMargin: 60
                }
            }
            HeaterV {
                type: 'MTR_nofc' //отключает контекст добавления на график
                name: "EK203"
                symbol: "EK203_205"
                Component.onCompleted: Logic.registerComponent(this)
                scale: 1
                valrotation: 0
                exist_freq: false
                anchors {
                    top: imgBlock1.top
                    topMargin: 610
                    left: imgBlock1.left
                    leftMargin: 440
                }
            }
            HeaterH {
                type: 'MTR_nofc' //отключает контекст добавления на график
                name: "EK204"
                symbol: "EK203_205"
                Component.onCompleted: Logic.registerComponent(this)
                scale: 1
                valrotation: 0
                exist_freq: false
                anchors {
                    top: imgBlock1.top
                    topMargin: 617
                    left: imgBlock1.left
                    leftMargin: 735
                }
            }
            ColumnLayout {
                width: 160
                height: 350
                spacing: 0
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 170
                    leftMargin: 770
                }
                Fan {
                    name: "ВМО1"
                    symbol: "M1"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 1
                    valrotation: 180
                    exist_freq: true
                }
                Fan {
                    type: 'MTR_nofc' //отключает контекст добавления на график
                    name: "ВМО2"
                    symbol: "M2"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 1
                    valrotation: 180
                    exist_freq: false
                }
                Fan {
                    name: "ВМО3"
                    symbol: "M3"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 1
                    valrotation: 180
                    exist_freq: true
                }
                Fan {
                    type: 'MTR_nofc' //отключает контекст добавления на график
                    name: "ВМО4"
                    symbol: "M4"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 1
                    valrotation: 180
                    exist_freq: false
                }
            }
            ColumnLayout {
                width: 180
                height: 180
                spacing: 0
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 682
                    leftMargin: 1573
                }
                HeaterH {
                    type: 'MTR_nofc' //отключает контекст добавления на график
                    name: "ЭНБМ1"
                    symbol: "EK201"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 1
                    valrotation: 0
                    exist_freq: false
                }
                HeaterH {
                    type: 'MTR_nofc' //отключает контекст добавления на график
                    name: "ЭНБМ2"
                    symbol: "EK202"
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 1
                    valrotation: 0
                    exist_freq: false
                }
            }
        }
    }
}
