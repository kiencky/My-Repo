//=================================================================================.
/// @file   NowPlaying.qml.
//=================================================================================.
import QtQuick
import MusicApp 1.0
import QtQuick.Layouts

Rectangle {
    color: Theme.background

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        Image {
            id: cover
            Layout.fillWidth: true
            Layout.fillHeight: true
            fillMode: Image.PreserveAspectFit
            asynchronous: true
            source: MusicController.currentCoverArt !== "" ?
                    MusicController.currentCoverArt
                    : "qrc:/assets/cover/disc.gif"
        }

        // AnimatedImage {
        //     source: "qrc:/assets/cover/disc.gif"
        //     cache: true
        //     smooth: true
        //     asynchronous: true
        //     playing: true
        // }

        Text {
            text: MusicController.currentTitle
            color: Theme.textPrimary
            font.pixelSize: 20
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: {
                const model = MusicController.songModel
                const index = MusicController.currentIndex

                if( !model || index < 0 || index >= model.count ) {
                    return "Unknown Artist"
                }

                return MusicController.currentArtist
            }

            color: Theme.textSecondary
            font.pixelSize: 15
            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
