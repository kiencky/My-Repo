import QtQuick

Rectangle {
    width: parent.width
    height: 30

    property bool hovered: false

    property string songName

    signal clicked(string name)

    color: hovered ? "#333" : "#555"

    Text {
        text: parent.songName
        anchors.centerIn: parent
        color: "white"
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: hovered = true
        onExited: hovered = false

        onClicked: {
            // click(parent.songName)
            console.log("Play:", parent.songName)
        }
    }
}
