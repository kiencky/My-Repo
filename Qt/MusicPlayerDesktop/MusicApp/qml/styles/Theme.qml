//=================================================================================.
/// @file   Theme.qml.
//=================================================================================.
pragma Singleton    // Allow to create the only object.
import QtQuick
// import MusicApp 1.0

QtObject {
    // Theme mode.
    property string mode: "light"     // "dark" or "light".

    // Background.
    property color background: mode === "light" ? Colors.gray100 : Colors.gray800
    property color cardBackground: mode === "light" ? Colors.white : "#303030"
    property color moduleBgr1: mode === "light" ? Colors.gray300 : "#303030"
    property color moduleBgr2: mode === "light" ? Colors.gray400 : "#303030"
    property color moduleBgr3: mode === "light" ? Colors.gray800 : "#303030"
    property color moduleBgr4: mode === "light" ? Colors.gray900 : "#303030"

    // Text.
    property color textPrimary: mode === "light" ? Colors.black : Colors.white
    property color textSecondary: mode === "light" ? "#333" : "#CCC"

    // Accent.
    property color accent: Colors.primary

    // Border.
    property color border: mode === "light" ? Colors.gray300 : "#505050"
}
