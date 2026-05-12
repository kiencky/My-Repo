//=================================================================================.
/// @file   SongItem.qml.
//=================================================================================.
import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import MusicApp 1.0

Rectangle {
    width: ListView.view.width
    implicitHeight: rowContent.implicitHeight + 5
    // border.width: 2

    // Check the song is playing now.
    /// "===" - absolute equality, return True if both value and data type is the same.
    property bool isActive: model.index === MusicController.currentIndex
    property bool ishovered: false
    property bool isWindowMaximized: (Window.window && Window.window.visibility) === Window.Maximized

    color: isActive? "#1A3E5C"
            : (ishovered ? "#202020" : "transparent")

    // Supplement function: 100000ms -> "1:40".
    function formatDuration(ms) {
        if (ms <= 0) {
            return "--:--"
        }

        var totalSec = Math.floor(ms / 1000)    // round down.
        var min = Math.floor(totalSec / 60)     // round down.
        var sec = totalSec % 60

        return min + ":" + (sec < 10 ? "0" : "") + sec      // m:ss
    }

    RowLayout {
        id: rowContent
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: 8
        spacing: 5

        //============== Column 1: The order number/ icon ================//
        Item {
            Layout.preferredWidth: parent.width * 0.1
            Layout.fillHeight: true

            AnimatedImage {
                anchors.centerIn: parent
                width: parent.width * 0.7
                height: parent.height * 0.7
                visible: isActive
                source: "qrc:/assets/icons/cd.gif"
                playing: isActive
            }

            Text {
                anchors.centerIn: parent
                // anchors.verticalCenter: parent.verticalCenter
                color: "white"
                visible: !isActive
                text: (model.index + 1).toString()
                font.pixelSize: isWindowMaximized ? 14 : 11
            }
        }

        //============== Column 3: Title + artist ================//
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            // Add the spacer to align buttons in the center.
            Item {
                Layout.preferredWidth: isWindowMaximized ? 5 : 2
            }

            // Title.
            Text {
                text: model.title
                color: isActive ? "#4A9EFF" : "white"
                font.pixelSize: isWindowMaximized ? 14 : 12
                font.weight: isActive ? Font.Medium : Font.Normal
                elide: Text.ElideRight      // Cut and replace by "..." if the name is too long.
                Layout.fillWidth: true
            }

            // Artist.
            Text {
                text: model.artist !== "" ? model.artist : "Unknown Artist"
                color: "#999"
                font.pixelSize: isWindowMaximized ? 12 : 11
                elide: Text.ElideRight      // Cut and replace by "..." if the name is too long.
                Layout.fillWidth: true
            }
        }

        //============== Column 4: Duration ================//
        Text {
            // Change format from "ms" to "mm:ss".
            text: formatDuration(model.duration)
            color: "white"
            font.pixelSize: 12
        }

        // Add the spacer to align buttons in the center.
        Item {
            Layout.fillWidth: true
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: ishovered = true
        onExited: ishovered = false
        onClicked: {
            MusicController.setIndex(model.index)
        }


    }
}
