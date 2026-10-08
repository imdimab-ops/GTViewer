import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Shapes
import "./components"
import "./js/connectionsLogic.js" as Logic

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
        console.log("QML GTU size:", width, height)
    }

    property bool _rotateOn : false
    property bool _fireOn : false
    property real _aiKC1: 190
    property real _aiKC2: 190
    property real _aiKC3: 190
    property real _aiKC4: 190
    property real _aiKC5: 190

    Connections {
        target: appEng
        function onFdataUpdateAI(arg) {
            var result = Logic.handleDataUpdateAI(arg)
            _rotateOn = result.rotateOn
            _fireOn = result.fireOn
            _aiKC1 = result.aiKC1
            _aiKC2 = result.aiKC2
            _aiKC3 = result.aiKC3
            _aiKC4 = result.aiKC4
            _aiKC5 = result.aiKC5
        }
    }
    Connections {
        target: appEng
        function onFdataUpdateMTR(arg) {
            var result = Logic.handleDataUpdateMTR(arg)
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
                source: "img/gtu/base.svg"
                sourceSize: Qt.size(2048, 1080) //Указываем явный размер, иначе Qt уменьшает SVG до размера экрана при загрузке

                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.color: "#cccccc"
                    border.width: 0
                    radius: 1
                }
            }

            AnimatedImage {
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/rotate.gif"
                sourceSize: Qt.size(475, 250)
                playing: true  //Автоматическое воспроизведение (!!!!!! Привязать в температуре в КС !!!!!!)
                visible: _rotateOn
                scale: 0.9
                anchors {
                    top: imgBlock1.top
                    topMargin: 575 //550
                    left: imgBlock1.left
                    leftMargin: 495 //470
                }

                //Если нужно вращение вокруг центра (по умолчанию вращение вокруг (0,0))
                transformOrigin: Item.Center  //Вращение вокруг центра элемента
                rotation: 0
                opacity: 1.0  //Полупрозрачность (0.0 - 1.0)
            }

            //Камера сгорания
            Image {
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/cam1.svg"
                sourceSize: Qt.size(225, 115) //Указываем явный размер, иначе Qt уменьшает SVG до размера экрана при загрузке
                visible: _fireOn
                scale: 1.0
                anchors {
                    top: imgBlock1.top
                    topMargin: 260
                    left: imgBlock1.left
                    leftMargin: 800
                }
            }
            Image {
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/cam1.svg"
                sourceSize: Qt.size(225, 115) //Указываем явный размер, иначе Qt уменьшает SVG до размера экрана при загрузке
                visible: _fireOn
                scale: 1.0
                anchors {
                    top: imgBlock1.top
                    topMargin: 380
                    left: imgBlock1.left
                    leftMargin: 800
                }
            }
            Image {
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/cam2.svg"
                sourceSize: Qt.size(235, 154) //Указываем явный размер, иначе Qt уменьшает SVG до размера экрана при загрузке
                visible: _fireOn
                scale: 1.0
                anchors {
                    top: imgBlock1.top
                    topMargin: 497
                    left: imgBlock1.left
                    leftMargin: 792
                }
            }
            Image {
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/cam3.svg"
                sourceSize: Qt.size(235, 154) //Указываем явный размер, иначе Qt уменьшает SVG до размера экрана при загрузке
                visible: _fireOn
                scale: 1.0
                anchors {
                    top: imgBlock1.top
                    topMargin: 747
                    left: imgBlock1.left
                    leftMargin: 792
                }
            }
            Image {
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/cam4.svg"
                sourceSize: Qt.size(225, 115) //Указываем явный размер, иначе Qt уменьшает SVG до размера экрана при загрузке
                visible: _fireOn
                scale: 1.0
                anchors {
                    top: imgBlock1.top
                    topMargin: 905
                    left: imgBlock1.left
                    leftMargin: 800
                }
            }
            Image {
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/turboFireEx.svg"
                sourceSize: Qt.size(375, 225) //Указываем явный размер, иначе Qt уменьшает SVG до размера экрана при загрузке
                visible: _fireOn
                scale: 1.05
                anchors {
                    top: imgBlock1.top
                    topMargin: 585
                    left: imgBlock1.left
                    leftMargin: 865
                }
            }
            AnimatedImage {
                width: 300
                height: 300
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/firecam.gif"
                playing: true  //Автоматическое воспроизведение (!!!!!! Привязать в температуре в КС !!!!!!)
                visible: Number(_aiKC1 >= 180) && _rotateOn
                scale: 0.7
                anchors {
                    top: imgBlock1.top
                    topMargin: 140
                    left: imgBlock1.left
                    leftMargin: 745
                }

                //Если нужно вращение вокруг центра (по умолчанию вращение вокруг (0,0))
                transformOrigin: Item.Center  //Вращение вокруг центра элемента
                rotation: 270
                opacity: 0.9  //Полупрозрачность (0.0 - 1.0)
            }
            AnimatedImage {
                width: 300
                height: 300
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/firecam.gif"
                playing: true  //Автоматическое воспроизведение (!!!!!! Привязать в температуре в КС !!!!!!)
                visible: Number(_aiKC2 >= 180) && _rotateOn
                scale: 0.7
                anchors {
                    top: imgBlock1.top
                    topMargin: 260
                    left: imgBlock1.left
                    leftMargin: 745
                }

                //Если нужно вращение вокруг центра (по умолчанию вращение вокруг (0,0))
                transformOrigin: Item.Center  //Вращение вокруг центра элемента
                rotation: 270
                opacity: 0.9  //Полупрозрачность (0.0 - 1.0)
            }
            AnimatedImage {
                width: 300
                height: 300
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/firecam.gif"
                playing: true  //Автоматическое воспроизведение (!!!!!! Привязать в температуре в КС !!!!!!)
                visible: Number(_aiKC3 >= 180) && _rotateOn
                scale: 0.7
                anchors {
                    top: imgBlock1.top
                    topMargin: 380
                    left: imgBlock1.left
                    leftMargin: 745
                }

                //Если нужно вращение вокруг центра (по умолчанию вращение вокруг (0,0))
                transformOrigin: Item.Center  //Вращение вокруг центра элемента
                rotation: 270
                opacity: 0.9  //Полупрозрачность (0.0 - 1.0)
            }
            AnimatedImage {
                width: 300
                height: 300
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/firecam.gif"
                playing: true  //Автоматическое воспроизведение (!!!!!! Привязать в температуре в КС !!!!!!)
                visible: Number(_aiKC4 >= 180) && _rotateOn
                scale: 0.7
                anchors {
                    top: imgBlock1.top
                    topMargin: 712
                    left: imgBlock1.left
                    leftMargin: 745
                }

                //Если нужно вращение вокруг центра (по умолчанию вращение вокруг (0,0))
                transformOrigin: Item.Center  //Вращение вокруг центра элемента
                rotation: 270
                opacity: 0.9  //Полупрозрачность (0.0 - 1.0)
            }
            AnimatedImage {
                width: 300
                height: 300
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/firecam.gif"
                playing: true  //Автоматическое воспроизведение (!!!!!! Привязать в температуре в КС !!!!!!)
                visible: Number(_aiKC5 >= 180) && _rotateOn
                scale: 0.7
                anchors {
                    top: imgBlock1.top
                    topMargin: 830
                    left: imgBlock1.left
                    leftMargin: 745
                }

                //Если нужно вращение вокруг центра (по умолчанию вращение вокруг (0,0))
                transformOrigin: Item.Center  //Вращение вокруг центра элемента
                rotation: 270
                opacity: 0.9  //Полупрозрачность (0.0 - 1.0)
            }
            AnimatedImage {
                z: -1
                width: 300
                height: 300
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                source: "img/gtu/fire.gif"
                playing: true  //Автоматическое воспроизведение (!!!!!! Привязать в температуре в КС !!!!!!)
                visible: _fireOn && _rotateOn
                scale: 1.2
                anchors {
                    top: imgBlock1.top
                    topMargin: 545
                    left: imgBlock1.left
                    leftMargin: 1000
                }

                //Если нужно вращение вокруг центра (по умолчанию вращение вокруг (0,0))
                transformOrigin: Item.Center  //Вращение вокруг центра элемента
                rotation: 90
                opacity: 1.0  //Полупрозрачность (0.0 - 1.0)
            }

            //Аналоговые параметры

            //=========== Метеостанция ========================
            Rectangle {
                width: 395
                height: 70
                color: "transparent"
                border.color: "#cccccc"
                border.width: 1
                anchors {
                    top: imgBlock1.top
                    topMargin: 30
                    left: imgBlock1.left
                    leftMargin: 30
                }

                AI {
                    name: "Ратм"
                    symbol: "PT908"
                    pv: 0
                    lrv: 0
                    urv: 200
                    stw: 0
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                    anchors {
                        top: parent.top
                        topMargin: 5
                        left: parent.left
                        leftMargin: 0
                    }
                }
                AI {
                    name: "Влажность"
                    symbol: "AT901"
                    pv: 0
                    lrv: 0
                    urv: 100
                    stw: 0
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                    anchors {
                        top: parent.top
                        topMargin: 5
                        left: parent.left
                        leftMargin: 120
                    }
                }
                AI {
                    name: "Tатм"
                    symbol: "TT903"
                    pv: 0
                    lrv: -40
                    urv: 110
                    stw: 0
                    Component.onCompleted: Logic.registerComponent(this)
                    scale: 0.70
                    anchors {
                        top: parent.top
                        topMargin: 5
                        left: parent.left
                        leftMargin: 240
                    }
                }
            }
            //======================================================

            //=========== "Электростартер" ========================
            Rectangle {
                color: "transparent"
                border.color: "#cccccc"
                border.width: 1
                implicitWidth: children[0].implicitWidth //children[0] - первый ребенок ColumnLayout
                implicitHeight: children[0].implicitHeight
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 460
                    leftMargin: 360
                }
                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 0

                    Label {
                        text: qsTr("Электростартер")
                        font.pixelSize: 12
                        color: "#333333"
                        Layout.alignment: Qt.AlignHCenter
                    }
                    AI {
                        name: "Nконтр"
                        symbol: "M801_2"
                        Component.onCompleted: Logic.registerComponent(this)
                        scale: 0.70
                        Layout.leftMargin: -12
                        Layout.rightMargin: -12
                    }
                    AI {
                        name: "Ток"
                        symbol: "OUT_CURR"
                        description: "Выходной ток"
                        Component.onCompleted: Logic.registerComponent(this)
                        scale: 0.70
                        Layout.leftMargin: -12
                        Layout.rightMargin: -12
                        Layout.topMargin: -12
                        Layout.bottomMargin: -12
                    }
                    AI {
                        name: "Момент"
                        symbol: "OUT_M"
                        description: "Выходной момент"
                        Component.onCompleted: Logic.registerComponent(this)
                        scale: 0.70
                        Layout.leftMargin: -12
                        Layout.rightMargin: -12
                    }
                }
            }
            //======================================================

            //=========== Топливная система ========================
            AI {
                name: "Ртг на фильтре"
                symbol: "PT421"
                pv: 0
                lrv: 0
                urv: 2
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 70
                    left: imgBlock1.left
                    leftMargin: 1875
                }
            }
            AI {
                name: "Ртг компр"
                symbol: "PT422"
                pv: 0
                lrv: 0
                urv: 2
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 70
                    left: imgBlock1.left
                    leftMargin: 1595
                }
            }
            AI {
                name: "Tтг компр"
                symbol: "TT421"
                pv: 0
                lrv: -50
                urv: 100
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 20
                    left: imgBlock1.left
                    leftMargin: 1595
                }
            }
            AI {
                name: "Расход ТГ"
                symbol: "FC424"
                pv: 0
                lrv: 0
                urv: 250
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 50
                    left: imgBlock1.left
                    leftMargin: 1450
                }
            }
            AI {
                name: "Aдг"
                symbol: "FV401_2"
                pv: 0
                lrv: 0
                urv: 2
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 70
                    left: imgBlock1.left
                    leftMargin: 1045
                }
            }
            AI {
                name: "Ртг FV401 т.1"
                symbol: "PT403"
                pv: 0
                lrv: 0
                urv: 2
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 70
                    left: imgBlock1.left
                    leftMargin: 1245
                }
            }
            AI {
                name: "Ртг FV401 т.2"
                symbol: "PT404"
                pv: 0
                lrv: 0
                urv: 2
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 20
                    left: imgBlock1.left
                    leftMargin: 1245
                }
            }
            AI {
                name: "Tтг"
                symbol: "TT401"
                pv: 0
                lrv: -50
                urv: 100
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 145
                    left: imgBlock1.left
                    leftMargin: 1245
                }
            }
            //======================================================

            //=========== Маслосистема =============================
            AI {
                name: "Уровень масла"
                symbol: "LT201"
                pv: 0
                lrv: 0
                urv: 500
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 1015
                    left: imgBlock1.left
                    leftMargin: 0
                }
            }
            AI {
                name: "Tмасла МБ"
                symbol: "TT204"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 965
                    left: imgBlock1.left
                    leftMargin: 00
                }
            }
            AI {
                name: "Рм"
                symbol: "PT203"
                pv: 0
                lrv: 0
                urv: 1
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 910
                    left: imgBlock1.left
                    leftMargin: 150
                }
            }
            AI {
                name: "Перепад"
                symbol: "PDT204"
                pv: 0
                lrv: 0
                urv: 160
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 965
                    left: imgBlock1.left
                    leftMargin: 265
                }
            }
            AI {
                name: "Рм ГТД_2"
                symbol: "PT206B"
                pv: 0
                lrv: 0
                urv: 630
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 850
                    left: imgBlock1.left
                    leftMargin: 470
                }
            }
            AI {
                name: "Рм ГТД_1"
                symbol: "PT206A"
                pv: 0
                lrv: 0
                urv: 630
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 895
                    left: imgBlock1.left
                    leftMargin: 470
                }
            }
            AI {
                name: "Tмвых АВОМ"
                symbol: "TT205"
                pv: 0
                lrv: -50
                urv: 100
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 940
                    left: imgBlock1.left
                    leftMargin: 470
                }
            }
            AI {
                name: "Tмвых ред"
                symbol: "TT206"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 770
                    left: imgBlock1.left
                    leftMargin: 385
                }
            }
            AI {
                name: "Tмвых ГТД"
                symbol: "TT207"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 770
                    left: imgBlock1.left
                    leftMargin: 620
                }
            }
            PumpLite {
                name: "Насос масла"
                symbol: "M201"
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.80
                anchors {
                    bottom: imgBlock1.bottom
                    bottomMargin: 55
                    left: imgBlock1.left
                    leftMargin: 185
                }
            }
            PumpLite {
                name: "Насос масла"
                symbol: "M202"
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.80
                anchors {
                    bottom: imgBlock1.bottom
                    bottomMargin: 0
                    left: imgBlock1.left
                    leftMargin: 185
                }
            }
            //======================================================

            //=========== ГТД =============================
            AI {
                name: "T КС1_1"
                symbol: "TE521"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 230
                    left: imgBlock1.left
                    leftMargin: 730
                }
            }
            AI {
                name: "T КС2_1"
                symbol: "TE522"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 345
                    left: imgBlock1.left
                    leftMargin: 730
                }
            }
            AI {
                name: "T КС3_1"
                symbol: "TE523"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 465
                    left: imgBlock1.left
                    leftMargin: 730
                }
            }
            AI {
                name: "T КС4_1"
                symbol: "TE524"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 800
                    left: imgBlock1.left
                    leftMargin: 730
                }
            }
            AI {
                name: "T КС5_1"
                symbol: "TE525"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 920
                    left: imgBlock1.left
                    leftMargin: 730
                }
            }
            AI {
                name: "T КС1_2"
                symbol: "TE531"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 300
                    left: imgBlock1.left
                    leftMargin: 850
                }
            }
            AI {
                name: "T КС2_2"
                symbol: "TE532"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 420
                    left: imgBlock1.left
                    leftMargin: 850
                }
            }
            AI {
                name: "T КС3_2"
                symbol: "TE533"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 540
                    left: imgBlock1.left
                    leftMargin: 850
                }
            }
            AI {
                name: "T КС4_2"
                symbol: "TE534"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 875
                    left: imgBlock1.left
                    leftMargin: 850
                }
            }
            AI {
                name: "T КС5_2"
                symbol: "TE535"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 990
                    left: imgBlock1.left
                    leftMargin: 850
                }
            }
            AI {
                name: "Обороты ГТД"
                symbol: "ST50"
                pv: 0
                lrv: 0
                urv: 100
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 645
                    left: imgBlock1.left
                    leftMargin: 730
                }
            }
            AI {
                name: "Ускорение ГТД"
                symbol: "SR50"
                pv: 0
                lrv: 0
                urv: 100
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 692
                    left: imgBlock1.left
                    leftMargin: 730
                }
            }
            AI {
                name: "T после компр"
                symbol: "TE701"
                pv: 0
                lrv: -50
                urv: 200
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 270
                    left: imgBlock1.left
                    leftMargin: 600
                }
            }
            AI {
                name: "Рк"
                symbol: "PT701"
                pv: 0
                lrv: 0
                urv: 1.6
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 320
                    left: imgBlock1.left
                    leftMargin: 600
                }
            }
            AI {
                name: "Gвх турбины"
                symbol: "FT901"
                pv: 0
                lrv: 0
                urv: 20
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 390
                    left: imgBlock1.left
                    leftMargin: 500
                }
            }
            AI {
                name: "Разр возд"
                symbol: "PDT902"
                pv: 0
                lrv: 0
                urv: 2.5
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 435
                    left: imgBlock1.left
                    leftMargin: 500
                }
            }
            AI {
                name: "Gвозд ГТД"
                symbol: "FC901"
                pv: 0
                lrv: 0
                urv: 15
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 480
                    left: imgBlock1.left
                    leftMargin: 500
                }
            }
            AI {
                name: "Tвх компр"
                symbol: "TT901"
                pv: 0
                lrv: -60
                urv: 60
                stw: 0
                Component.onCompleted: Logic.registerComponent(this)
                scale: 0.70
                anchors {
                    top: imgBlock1.top
                    topMargin: 525
                    left: imgBlock1.left
                    leftMargin: 500
                }
            }
            //======================================================

            //=========== Генератор AIpbv =====================
            Rectangle {
                width: mainRowGenerator.implicitWidth + 20 //растягиваем Rectangle по содержимому
                height: mainRowGenerator.implicitHeight + 30
                color: "transparent"
                border.color: "#cccccc"
                border.width: 1
                anchors {
                    top: imgBlock1.top
                    topMargin: 180
                    right: imgBlock1.right
                    rightMargin: 20
                }

                Label {
                    text: qsTr("Генератор")
                    font.pixelSize: 16
                    font.bold: true
                    color: "#333333"
                    anchors {
                        top: parent.top
                        topMargin: 0
                        horizontalCenter: parent.horizontalCenter
                    }
                }
                RowLayout {
                    id: mainRowGenerator
                    anchors {
                        top: parent.top
                        topMargin: 25
                        horizontalCenter: parent.horizontalCenter
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignTop
                        Label {
                            text: qsTr("Виброскорость, СКЗ")
                            font.pixelSize: 12
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        RowLayout {
                            spacing: 10
                            Layout.alignment: Qt.AlignHCenter
                            AIpbv {
                                name: "X, NDE"
                                symbol: "VT503"
                                pv: 78
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "Y, NDE"
                                symbol: "VT504"
                                pv: 78
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "X, DE"
                                symbol: "VT501"
                                pv: 178
                                lrv: 0
                                urv: 250
                                stw: 0
                                unit: "СКЗ"
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "Y, DE"
                                symbol: "VT502"
                                pv: 178
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }

                        }

                    }
                    Item {
                        width: 15
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignTop
                        Label {
                            text: qsTr("T обмоток, ℃")
                            font.pixelSize: 12
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        RowLayout {
                            spacing: 10
                            Layout.alignment: Qt.AlignHCenter
                            AIpbv {
                                name: "U, т.1"
                                symbol: "TE590"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "U, т.2"
                                symbol: "TE591"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "V, т.1"
                                symbol: "TE592"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "V, т.2"
                                symbol: "TE593"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "W, т.1"
                                symbol: "TE594"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "W, т.2"
                                symbol: "TE595"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                        }

                    }
                    Item {
                        width: 15
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignTop
                        Label {
                            text: qsTr("T подшипн, ℃")
                            font.pixelSize: 12
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        RowLayout {
                            spacing: 10
                            Layout.alignment: Qt.AlignHCenter
                            AIpbv {
                                name: "NDE"
                                symbol: "TE596"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                                onEmitAddToChart: (data)=> {
                                                      _addCharForPlot(data)
                                                  }
                            }
                            AIpbv {
                                name: "DE"
                                symbol: "TE597"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                        }
                    }
                }

            }
            //======================================================

            //=========== Редуктор AIpbv ======================
            Rectangle {
                width: mainRowReductor.implicitWidth + 20 //растягиваем Rectangle по содержимому
                height: mainRowReductor.implicitHeight + 30
                color: "transparent"
                border.color: "#cccccc"
                border.width: 1
                anchors {
                    top: imgBlock1.top
                    topMargin: 450
                    right: imgBlock1.right
                    rightMargin: 20
                }

                Label {
                    text: qsTr("Редуктор")
                    font.pixelSize: 16
                    font.bold: true
                    color: "#333333"
                    anchors {
                        top: parent.top
                        topMargin: 0
                        horizontalCenter: parent.horizontalCenter
                    }
                }
                RowLayout {
                    id: mainRowReductor
                    anchors {
                        top: parent.top
                        topMargin: 25
                        horizontalCenter: parent.horizontalCenter
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignTop
                        Label {
                            text: qsTr("Виброскорость, мм/с")
                            font.pixelSize: 12
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        RowLayout {
                            spacing: 10
                            Layout.alignment: Qt.AlignHCenter
                            AIpbv {
                                name: "X"
                                symbol: "VT505"
                                pv: 78
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "Y"
                                symbol: "VT506"
                                pv: 78
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "Z"
                                symbol: "VT510"
                                pv: 178
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                        }
                    }
                    Item {
                        width: 20
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignTop
                        Label {
                            text: qsTr("Вибропер, мкм")
                            font.pixelSize: 12
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        RowLayout {
                            spacing: 10
                            Layout.alignment: Qt.AlignHCenter
                            AIpbv {
                                name: "НВ т.1"
                                symbol: "ZT501"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "НВ т.2"
                                symbol: "ZT502"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                        }
                    }
                    Item {
                        width: 20
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignTop
                        Label {
                            text: qsTr("T подшипн, ℃")
                            font.pixelSize: 12
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        RowLayout {
                            spacing: 10
                            Layout.alignment: Qt.AlignHCenter
                            AIpbv {
                                name: "ПВвх т.1"
                                symbol: "TE576"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "ПВвх т.2"
                                symbol: "TE577"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "ПВвх т.3"
                                symbol: "TE578"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "ПВвых т.1"
                                symbol: "TE579"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "ПВвых т.2"
                                symbol: "TE580"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "ПВвых т.3"
                                symbol: "TE581"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "НВвх"
                                symbol: "TE582"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                                onEmitAddToChart: (data)=> {
                                                      _addCharForPlot(data)
                                                  }
                            }
                            AIpbv {
                                name: "НВвых"
                                symbol: "TE583"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                        }
                    }
                }
            }
            //======================================================

            //=========== Турбина AIpbv =======================
            Rectangle {
                width: mainRowGTD.implicitWidth + 20 //растягиваем Rectangle по содержимому
                height: mainRowGTD.implicitHeight + 30
                color: "transparent"
                border.color: "#cccccc"
                border.width: 1
                anchors {
                    top: imgBlock1.top
                    topMargin: 735
                    right: imgBlock1.right
                    rightMargin: 20
                }

                Label {
                    text: qsTr("ГТД")
                    font.pixelSize: 16
                    font.bold: true
                    color: "#333333"
                    anchors {
                        top: parent.top
                        topMargin: 0
                        horizontalCenter: parent.horizontalCenter
                    }
                }
                RowLayout {
                    id: mainRowGTD
                    anchors {
                        top: parent.top
                        topMargin: 25
                        horizontalCenter: parent.horizontalCenter
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignTop
                        Label {
                            text: qsTr("...")
                            font.pixelSize: 12
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        RowLayout {
                            spacing: 10
                            Layout.alignment: Qt.AlignHCenter
                            AIpbv {
                                name: "Gтг"
                                symbol: "FC424"
                                pv: 78
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "Gвозд"
                                symbol: "FC901"
                                pv: 78
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "Нагр."
                                symbol: "PLoad"
                                pv: 178
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                        }
                    }
                    Item {
                        width: 15
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignTop
                        Label {
                            text: qsTr("Виброскорость, мм/с")
                            font.pixelSize: 12
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        RowLayout {
                            spacing: 10
                            Layout.alignment: Qt.AlignHCenter
                            AIpbv {
                                name: "X"
                                symbol: "VT507"
                                pv: 78
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "Y"
                                symbol: "VT508"
                                pv: 78
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "Z"
                                symbol: "VT509"
                                pv: 178
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                        }
                    }
                    Item {
                        width: 15
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignTop
                        Label {
                            text: qsTr("Вибропер, мкм")
                            font.pixelSize: 12
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        RowLayout {
                            spacing: 10
                            Layout.alignment: Qt.AlignHCenter
                            AIpbv {
                                name: "т.1"
                                symbol: "ZT503"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "т.2"
                                symbol: "ZT504"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                        }
                    }
                    Item {
                        width: 15
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignTop
                        Label {
                            text: qsTr("T подшипн, ℃")
                            font.pixelSize: 12
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }
                        RowLayout {
                            spacing: 10
                            Layout.alignment: Qt.AlignHCenter
                            AIpbv {
                                name: "ОК ОП т.1"
                                symbol: "TE501A"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "ОК ОП т.2"
                                symbol: "TE501B"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "ОК ОУП т.1"
                                symbol: "TE502A"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "ОК ОУП т.2"
                                symbol: "TE502B"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "УРК ОУП т.1"
                                symbol: "TE503A"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "УРК ОУП т.2"
                                symbol: "TE503B"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "УУК ОУП т.1"
                                symbol: "TE504A"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                            AIpbv {
                                name: "УУК ОУП т.2"
                                symbol: "TE504B"
                                pv: 20
                                lrv: 0
                                urv: 250
                                stw: 0
                                Layout.alignment: Qt.AlignTop
                                Component.onCompleted: Logic.registerComponent(this)
                            }
                        }
                    }
                }
            }
            //======================================================
        }
    }
}
