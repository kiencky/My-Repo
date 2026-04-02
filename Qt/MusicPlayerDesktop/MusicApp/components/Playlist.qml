import QtQuick
import QtQuick.Controls

Rectangle {
    color: "#333"

    // Create the signal.
    signal songSelected(string name)

    // ListView to store song data.
    ListView {
        anchors.fill: parent

        model: ListModel {
            ListElement { name: "Song A" }
            ListElement { name: "Song B" }
            ListElement { name: "Song C" }
        }

        delegate: SongItem {
            songName: name

            // Emit signal when clicking.
            onClicked: {
                songSelected( songName )
            }
        }
    }
}
