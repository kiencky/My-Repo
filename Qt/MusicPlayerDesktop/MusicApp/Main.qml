import QtQuick
import QtQuick.Controls
import "components"

ApplicationWindow {
    width: 900
    height: 500
    visible: true
    title: qsTr("Music Player")                     // Support translation into other languagues.

    property string currentSong: "No song"          // Current song.

    Row {
        anchors.fill: parent

        // LEFT: Playlist.
        Playlist {
            id: playlist
            width: parent.width*0.25
            height: parent.height

            onSongSelected: function(name) {
                currentSong = name
            }
        }

        Column {
            width: parent.width*0.75
            height: parent.height
            spacing: 5

            // TOP: The current song is currently playing.
            NowPlaying {
                width: parent.width
                height: parent.height*0.7
                songName: currentSong
            }

            // BOTTOM: The music control bar.
            PlayerControls {
                width: parent.width
                height: parent.height*0.3
            }
        }
    }
}
