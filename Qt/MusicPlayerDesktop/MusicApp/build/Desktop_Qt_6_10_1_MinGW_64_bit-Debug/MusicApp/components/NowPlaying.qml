import QtQuick

Rectangle {
    color: "gray"
    // Declare variable to store song name.
    property string songName

    Column {
        anchors.centerIn: parent
        spacing: 10

        Text {
            text: qsTr("Now Playing")
            color: "white"
            font.pixelSize: 20
        }

        Text {
            text: songName
            color: "gray"
            font.pixelSize: 20
        }
    }
}
