//=================================================================================.
/// @file   Main.qml.
//=================================================================================.
import QtQuick
import QtQuick.Controls
import MusicApp 1.0
import "components"

ApplicationWindow {
    width: 900
    height: 500
    visible: true
    title: qsTr("Music Player")                     // Support translation into other languagues.

    // Scan music file as soon as the component is initialized.
    Component.onCompleted: {
        MusicController.scanMusicDirectory()
        Theme.mode = MusicController.theme
    }

    Row {
        anchors.fill: parent

        // LEFT: Playlist.
        Playlist {
            id: playlist
            width: parent.width*0.25
            height: parent.height

        }

        Column {
            width: parent.width*0.75
            height: parent.height
            spacing: 5

            // TOP: The current song is currently playing.
            NowPlaying {
                width: parent.width
                height: parent.height*0.7
            }

            // BOTTOM: The music control bar.
            PlayerControls {
                width: parent.width
                height: parent.height*0.3
            }
        }
    }
}
