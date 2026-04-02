import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: "light blue"

    Column {
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        spacing: 5

        // Layout for buttons.
        Rectangle {
            width: parent.width
            height: parent.height*0.6
            color: "#999"

            RowLayout {
                anchors.fill: parent
                spacing: 20

                // Add the spacer to align buttons in the center.
                Item {
                    Layout.fillWidth: true
                }

                Rectangle {
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                    radius: width/2                             // Create a circle with radius is width/2.
                    color: mouseBt1.pressed? "#555"             // Change color when pressing.
                            : mouseBt1.containsMouse? "#777"    // Change color when hovering.
                            : "white"

                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: parent.width/2
                        text: "⏮"
                    }

                    MouseArea {
                        id: mouseBt1
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: console.log("Prev")
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 60
                    radius: width/2                             // Create a circle with radius is width/2.
                    color: mouseBt2.pressed? "#555"             // Change color when pressing.
                            : mouseBt2.containsMouse? "#777"    // Change color when hovering.
                            : "white"

                    property bool isPlaying: false

                    Text {
                        id: text
                        anchors.centerIn: parent
                        font.pixelSize: parent.width/2
                        text: parent.isPlaying? "⏸" : "▶"
                    }

                    MouseArea {
                        id: mouseBt2
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            parent.isPlaying = !parent.isPlaying
                            console.log( parent.isPlaying? "Play" : "Pause" )
                        }
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                    radius: width/2                             // Create a circle with radius is width/2.
                    color: mouseBt3.pressed? "#555"             // Change color when pressing.
                            : mouseBt3.containsMouse? "#777"    // Change color when hovering.
                            : "white"

                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: parent.width/2
                        text: "⏭"
                    }

                    MouseArea {
                        id: mouseBt3
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: console.log("Next")
                    }
                }

                // Add the spacer to align buttons in the center.
                Item {
                    Layout.fillWidth: true
                }
            }
        }

        Rectangle {
            width: parent.width
            height: parent.height*0.4 - 5
            color: "#999"

            Slider {
                anchors.centerIn: parent
                width: parent.width*0.9
                from: 0
                to: 100
                value: 10

                onMoved: {
                    console.log("Seek:", value)
                }
            }
        }
    }
}
