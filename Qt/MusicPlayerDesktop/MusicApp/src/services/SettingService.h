//================================================================================================
/// @file   SettingService.h.
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

#ifndef __SETTINGSERVICE_H__
#define __SETTINGSERVICE_H__

#include <QObject>
#include <QSettings>

class SettingService : public QObject
{
    Q_OBJECT
public:
    explicit SettingService( QObject* parent = nullptr );

//======== SETTER ==========//
    void setVolume( double value );             // Volume: 0.0 ? 1.0.
    void setTheme( const QString &value );      // Theme: "light" / "dark".
    void setLastSongIndex( int index );         // Last played index.
    void setLastSongPosition( qint64 pos );     // Last played position (ms).

//======== GETTER ==========//
    double volume() const;                      // Volume: 0.0 ? 1.0.
    QString theme() const;          // Theme: "light" / "dark".
    int lastSongIndex() const;                  // Last played index.
    qint64 lastSongPosition() const;            // Last played position (ms).

private:
    QSettings m_settings;

};

#endif // __SETTINGSERVICE_H__
