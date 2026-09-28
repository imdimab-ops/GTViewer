import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#f0f0f0"
    
    // Свойства для связи с C++
    property string screenName: "Main"
    
    // Доступ к бэкенду из C++
    Text {
        id: titleText
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: "SCADA Screen - " + screenName
        font.pixelSize: 24
        font.bold: true
        color: "#333333"
    }
}
