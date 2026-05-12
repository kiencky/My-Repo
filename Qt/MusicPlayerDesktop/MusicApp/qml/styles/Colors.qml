//=================================================================================.
/// @file   Colors.qml.
//=================================================================================.
pragma Singleton    // Allow to create the only object.
import QtQuick

QtObject {
    // Primary brand colors
    property color primary: "#4CAF50"
    property color secondary: "#8BC34A"

    // Neutral grayscale
    property color white: "#FFFFFF"
    property color black: "#000000"

    property color gray50:  "#FAFAFA"
    property color gray100: "#F5F5F5"
    property color gray200: "#EEEEEE"
    property color gray300: "#E0E0E0"
    property color gray400: "#BDBDBD"
    property color gray500: "#9E9E9E"
    property color gray600: "#757575"
    property color gray700: "#616161"
    property color gray800: "#424242"
    property color gray900: "#212121"

    // Error / Warning / Success
    property color error: "#F44336"
    property color warning: "#FF9800"
    property color success: "#4CAF50"
}
