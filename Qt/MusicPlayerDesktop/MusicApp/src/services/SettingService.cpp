//================================================================================================
/// @file   SettingService.cpp.
/// @brief  Save user setting status (song index, position, volumn, ...).
/*
Note:
    - Use QSetting to save status file.
        Windows: Registry.
        Linux: ~/.config/AppName.ini.
        MacOS: ~/Library/Preferences.
    - Automatically loading when lauching app.
    - Automatically saving when closing app.
*/
//================================================================================================

#include "SettingService.h"

// Function---------------------------------------------------.
// Summary: Constructor.
// Note:    - orgName: KienPQ, appName: MusicPlayer
//          - Registry path: HKEY_CURRENT_USER\Software\KienPQ\MusicPlayer
SettingService::SettingService( QObject *parent )
    : QObject( parent )
    , m_settings( "KienPQ", "MusicPlayer" )
{}

// Function---------------------------------------------------.
// Summary: Get volume value in the registry.
// Param:   None.
// Return:  .
// Note:    - If no value is saved, return 0.7.
//          - player: group, volume: key.
//          - Path: HKEY_CURRENT_USER\Software\KienPQ\MusicPlayer\player\volume.
double SettingService::volume() const
{
    return m_settings.value("player/volume", 0.7).toDouble();
}

// Function---------------------------------------------------.
// Summary: Set volume value in the registry.
// Param:   .
// Return:  None.
// Note:    - player: group, volume: key.
//          - Path: HKEY_CURRENT_USER\Software\KienPQ\MusicPlayer\player\volume.
void SettingService::setVolume(double value)
{
    m_settings.setValue("player/volume", value);
    m_settings.sync();
}

// Function---------------------------------------------------.
// Summary: Get the theme value.
// Param:   .
// Return:  .
// Note:    - If no value is saved, return "light".
//          - ui: group, theme: key.
//          - Path: HKEY_CURRENT_USER\Software\KienPQ\MusicPlayer\ui\theme.
QString SettingService::theme() const
{
    return m_settings.value("ui/theme", "light").toString();
}

// Function---------------------------------------------------.
// Summary: Set theme value in the registry.
// Param:   .
// Return:  None.
// Note:    - ui: group, theme: key.
//          - Path: HKEY_CURRENT_USER\Software\KienPQ\MusicPlayer\ui\theme.
void SettingService::setTheme(const QString &value)
{
    m_settings.setValue("ui/theme", value);
    m_settings.sync();
}

// Function---------------------------------------------------.
// Summary: Get the last song index in the registry.
// Param:   .
// Return:  .
// Note:    - player: group, lastIndex: key.
//          - Path: HKEY_CURRENT_USER\Software\KienPQ\MusicPlayer\player\lastIndex.
int SettingService::lastSongIndex() const
{
    return m_settings.value("player/lastIndex", 0).toInt();
}

// Function---------------------------------------------------.
// Summary: Set the last song index in the registry.
// Param:   .
// Return:  .
// Note:    - player: group, lastIndex: key.
//          - Path: HKEY_CURRENT_USER\Software\KienPQ\MusicPlayer\player\lastIndex.
void SettingService::setLastSongIndex(int index)
{
    m_settings.setValue("player/lastIndex", index);
    m_settings.sync();
}

// Function---------------------------------------------------.
// Summary: Set the last song position in the registry.
// Param:   .
// Return:  .
// Note:    - player: group, lastPosition: key.
//          - Path: HKEY_CURRENT_USER\Software\KienPQ\MusicPlayer\player\lastPosition.
qint64 SettingService::lastSongPosition() const
{
    return m_settings.value("player/lastPosition", 0).toLongLong();
}

// Function---------------------------------------------------.
// Summary: Get the last song position in the registry.
// Param:   .
// Return:  .
// Note:    - player: group, lastPosition: key.
//          - Path: HKEY_CURRENT_USER\Software\KienPQ\MusicPlayer\player\lastPosition.
void SettingService::setLastSongPosition(qint64 pos)
{
    m_settings.setValue("player/lastPosition", pos);
    m_settings.sync();
}