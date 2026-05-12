//=================================================================================.
/// @file   PlayerControls.qml.
//=================================================================================.
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MusicApp 1.0

Rectangle {
    property double lastVolume: 1.0

    function formatDuration(ms) {
        if (ms <= 0) {
            return "--:--"
        }

        var totalSec = Math.floor(ms / 1000)    // round down.
        var min = Math.floor(totalSec / 60)     // round down.
        var sec = totalSec % 60

        return min + ":" + (sec < 10 ? "0" : "") + sec      // m:ss
    }

    Column {
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        spacing: 2

        // Layout for buttons.
        Rectangle {
            id: btnLayout
            width: parent.width
            height: parent.height * 0.6
            color: Theme.moduleBgr1

            RowLayout {
                anchors.fill: parent
                spacing: btnLayout.width * 0.03

                // Add the spacer to align buttons in the center.
                Item {
                    Layout.preferredWidth: btnLayout.width * 0.25
                }

                // Shuffle button.
                Button {
                    id: shuffleBtn
                    Layout.preferredWidth: parent.height * 0.35
                    Layout.preferredHeight: parent.height * 0.35

                    flat: true          // Disable the defalt styles of button.
                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: MusicController.shuffle ? "Not Shuffle" : "Shuffle"
                    ToolTip.delay: 500          // Delay for displaying 500ms.
                    // ToolTip.timeout: 1000       // Disappear after 1000ms.

                    background: Rectangle {
                        anchors.fill: parent
                        radius: width / 2
                        // color: MusicController.shuffle ? "#3344aaff" : "transparent"
                        color: shuffleBtn.down ? "#777" :
                               shuffleBtn.hovered ? "#888" :
                               MusicController.shuffle ? "#3344aaff" : "transparent"
                               
                        border.color: "deepskyblue"
                        border.width: MusicController.shuffle ? 1 : 0
                    }

                    onClicked: {
                        MusicController.shuffle = !MusicController.shuffle
                    }

                    contentItem: Image {
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit       // Keep image aspect ratio.
                        // opacity: MusicController.shuffle ? 0.1 : 0.7
                        source: "qrc:/assets/icons/shuffle.png"
                    }
                }

                // Previous Button.
                Button {
                    id: previousBtn
                    Layout.preferredWidth: parent.height * 0.4
                    Layout.preferredHeight: parent.height * 0.4

                    background: Rectangle {
                        anchors.fill: parent
                        radius: width / 2
                        color: previousBtn.down ? "#777" :
                               previousBtn.hovered ? "#888" : "transparent"
                    }

                    flat: true          // Disable the defalt styles of button.
                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: "Previous"
                    ToolTip.delay: 500          // Delay for displaying 500ms.
                    // ToolTip.timeout: 1000       // Disappear after 1000ms.

                    contentItem: Image {
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit       // Keep image aspect ratio.
                        source: "qrc:/assets/icons/prev.png"    // "qrc:" Configure Qt to find file in packaged resource.
                    }

                    onClicked: {
                        MusicController.previous()
                    }
                }

                // Play/Pause Button.
                Button {
                    id: playBtn
                    Layout.preferredWidth: parent.height * 0.45
                    Layout.preferredHeight: parent.height * 0.45

                    background: Rectangle {
                        anchors.centerIn: parent
                        width: playBtn.width * 1.2
                        height: playBtn.height * 1.2
                        radius: width / 2
                        color: playBtn.down ? "#777" :
                               playBtn.hovered ? "#888" : "transparent"
                        border.color: "deepskyblue"
                        border.width: 1
                    }

                    flat: true          // Disable the defalt styles of button.
                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: MusicController.isPlaying ? "Pause" : "Play"
                    ToolTip.delay: 500          // Delay for displaying 500ms.
                    // ToolTip.timeout: 1000       // Disappear after 1000ms.

                    contentItem: Image {
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit       // Keep image aspect ratio.
                        source: MusicController.isPlaying ?
                                  "qrc:/assets/icons/pause.png"
                                  : "qrc:/assets/icons/play.png"
                    }

                    onClicked: {
                        MusicController.playOrPause()
                    }
                }

                // Next Button.
                Button {
                    id: nextBtn
                    Layout.preferredWidth: parent.height * 0.4
                    Layout.preferredHeight: parent.height * 0.4

                    background: Rectangle {
                        anchors.fill: parent
                        radius: width / 2
                        color: nextBtn.down ? "#777" :
                               nextBtn.hovered ? "#888" : "transparent"
                    }

                    flat: true          // Disable the defalt styles of button.
                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: "Next"
                    ToolTip.delay: 500          // Delay for displaying 500ms.
                    // ToolTip.timeout: 1000       // Disappear after 1000ms.

                    contentItem: Image {
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit       // Keep image aspect ratio.
                        source: "qrc:/assets/icons/next.png"    // "qrc:" Configure Qt to find file in packaged resource.
                    }

                    onClicked: {
                        MusicController.next()
                    }
                }

                // Repeat Mode.
                Button {
                    id: repeatBtn
                    Layout.preferredWidth: parent.height * 0.35
                    Layout.preferredHeight: parent.height * 0.35

                    flat: true          // Disable the defalt styles of button.
                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: MusicController.repeatMode === MusicController.NotRepeat ? "Repeat All"
                                 : (MusicController.repeatMode === MusicController.RepeatAll ? "Repeat Once"
                                 : "Not Repeat")
                    ToolTip.delay: 500          // Delay for displaying 500ms.
                    // ToolTip.timeout: 1000       // Disappear after 1000ms.

                    background: Rectangle {
                        anchors.fill: parent
                        radius: width / 2
                        color: repeatBtn.down ? "#777" :
                               repeatBtn.hovered ? "#888" :
                               MusicController.repeatMode === MusicController.NotRepeat ?
                                "transparent" : "#3344aaff"
                        border.color: "deepskyblue"
                        border.width: (MusicController.repeatMode === MusicController.NotRepeat) ? 0 : 1
                    }

                    onClicked: {
                        MusicController.repeatMode = (MusicController.repeatMode + 1) % 3
                    }

                    contentItem: Image {
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit       // Keep image aspect ratio.
                        opacity: (MusicController.repeatMode === MusicController.NotRepeat) ? 0.7 : 1.0

                        source: {
                            switch (MusicController.repeatMode) {
                                case MusicController.NotRepeat:
                                    return "qrc:/assets/icons/notRepeat.png"
                                case MusicController.RepeatAll:
                                    return "qrc:/assets/icons/repeatAll.png"
                                case MusicController.RepeatOne:
                                    return "qrc:/assets/icons/repeatOne.png"
                                default:
                                    return ""
                            }
                        }
                    }
                }

                // Add the spacer to align buttons in the center.
                Item {
                    Layout.preferredWidth: btnLayout.width * 0.05
                }

                RowLayout {
                    spacing: btnLayout.width * 0.01

                    // Volume.
                    Button {
                        id: volumeBtn
                        Layout.preferredWidth: btnLayout.height * 0.2
                        Layout.preferredHeight: btnLayout.height * 0.2

                        background: Rectangle {
                            anchors.fill: parent
                            radius: width / 2
                            color: volumeBtn.down ? "#777" :
                                volumeBtn.hovered ? "#888" : "transparent"
                            border.color: "deepskyblue"
                            border.width: MusicController.volume === 0.0 ? 1 : 0
                        }

                        flat: true          // Disable the defalt styles of button.
                        hoverEnabled: true
                        ToolTip.visible: hovered
                        ToolTip.text: MusicController.volume === 0.0 ? "Unmute" : "Mute"
                        ToolTip.delay: 500          // Delay for displaying 500ms.
                        // ToolTip.timeout: 1000       // Disappear after 1000ms.

                        contentItem: Image {
                            anchors.centerIn: parent
                            fillMode: Image.PreserveAspectFit       // Keep image aspect ratio.
                            source: MusicController.volume === 0.0 ?
                                    "qrc:/assets/icons/mute.png"
                                    : "qrc:/assets/icons/sound.png"
                        }

                        onClicked: {
                            if( MusicController.volume === 0.0 ) {
                                // Unmute.
                                MusicController.volume = lastVolume
                            } else {
                                // Mute.
                                lastVolume = MusicController.volume
                                MusicController.volume = 0.0
                            }
                        }
                    }

                    // Volume slider.
                    Slider {
                        id: volumeSlider
                        Layout.preferredWidth: btnLayout.width * 0.1
                        from: 0.0
                        to: 1.0
                        value: MusicController.volume

                        // Track on Slider.
                        background: Rectangle {
                            x: 0
                            y: parent.height / 2 - height / 2
                            width: parent.width
                            height: 4
                            radius: 2
                            color: "#666666"

                            // For the left of the track.
                            Rectangle {
                                anchors.verticalCenter: parent.verticalCenter
                                height: parent.height
                                radius: parent.radius
                                width: parent.width * ((volumeSlider.value - volumeSlider.from)
                                                    / ((volumeSlider.to - volumeSlider.from) || 1))
                                color: "#00A1FF"
                            }
                        }

                        // Handle.
                        handle: Rectangle {
                            implicitWidth: 14
                            implicitHeight: 14
                            radius: 7
                            color: "#ffffff"
                            border.color: "#00A1FF"
                            border.width: 2
                            anchors.verticalCenter: parent.verticalCenter

                            x: volumeSlider.leftPadding + volumeSlider.visualPosition*( volumeSlider.availableWidth - width)
                        }

                        onMoved: {
                            MusicController.volume = value
                        }
                    }
                }
                
                // Add the spacer to align buttons in the center.
                Item {
                    Layout.preferredWidth: btnLayout.width * 0.01
                }
            }
        }

        // Duration Slider.
        Rectangle {
            width: parent.width
            height: parent.height * 0.4
            color: Theme.moduleBgr2

            RowLayout {
                spacing: 10
                anchors.fill: parent

                // Add the spacer to align buttons in the center.
                Item {
                    Layout.preferredWidth: 30
                }

                // Position.
                Text {
                    text: formatDuration( MusicController.position )
                    font.pixelSize: 15
                    Layout.preferredWidth: 30
                }

                Slider {
                    id: durationSlider
                    Layout.fillWidth: true
                    from: 0
                    to: MusicController.duration
                    value: MusicController.position

                    // Track on Slider.
                    background: Rectangle {
                        x: 0
                        y: parent.height / 2 - height / 2
                        width: parent.width
                        height: 4
                        radius: 2
                        color: "#666666"

                        // For the left of the track.
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            height: parent.height
                            radius: parent.radius
                            width: parent.width * ((durationSlider.value - durationSlider.from) 
                                                / (durationSlider.to - durationSlider.from || 1))
                            color: "#00A1FF"
                        }
                    }

                    // Handle.
                    handle: Rectangle {
                        implicitWidth: 14
                        implicitHeight: 14
                        radius: 7
                        color: "#ffffff"
                        border.color: "#00A1FF"
                        border.width: 2
                        anchors.verticalCenter: parent.verticalCenter

                        x: durationSlider.leftPadding + durationSlider.visualPosition*( durationSlider.availableWidth - width)
                    }

                    onMoved: {
                        MusicController.setPosition(value)
                    }
                }

                // Duration.
                Text {
                    text: formatDuration( MusicController.duration )
                    font.pixelSize: 15
                    Layout.preferredWidth: 30
                }

                // Add the spacer to align buttons in the center.
                Item {
                    Layout.preferredWidth: 30
                }
            }
        }
    }
}
