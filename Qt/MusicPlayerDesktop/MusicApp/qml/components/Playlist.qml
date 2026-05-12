//=================================================================================.
/// @file   Playlist.qml.
//=================================================================================.
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import MusicApp 1.0

Rectangle {
    color: Theme.moduleBgr3

    ColumnLayout {
        anchors.fill: parent
        spacing: 2

        Rectangle {
            id: titleBar
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height * 0.04
            color: Theme.moduleBgr4

            RowLayout {
                anchors.fill: parent
                // anchors.leftMargin: 2
                // anchors.rightMargin: 2

                Item {
                    Layout.preferredWidth: parent.width * 0.1
                    Layout.fillHeight: true

                    Text {
                        anchors.centerIn: parent
                        anchors.verticalCenter: parent.verticalCenter
                        text: "No"
                        color: "white"
                        font.pixelSize: Window.window.visibility === Window.Maximized ? 13 : 11
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin: 5

                        text: "Song Name"
                        color: "white"
                        font.pixelSize: Window.window.visibility === Window.Maximized ? 13 : 11
                    }
                }

                Item {
                    Layout.preferredWidth: parent.width * 0.2
                    Layout.fillHeight: true

                    Text {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        // anchors.rightMargin: 2

                        text: "Duration"
                        color: "white"
                        font.pixelSize: Window.window.visibility === Window.Maximized ? 13 : 11
                    }
                }

                // Add the spacer to align buttons in the center.
                Item {
                    Layout.preferredWidth: titleBar.width * 0.02
                }
            }
        }

        // ListView to store song data.
        ListView {
            // anchors.fill: parent
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            // Automatically call rowCount() and data() to get data for model.
            model: MusicController.songModel

            delegate: SongItem { }
        }
    }
}
