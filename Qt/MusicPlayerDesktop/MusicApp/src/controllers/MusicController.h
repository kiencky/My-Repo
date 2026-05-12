//================================================================================================
/// @file   MusicController.h.
/// @brief  Declare the class controlling logic for music app.
/*
Detail:
- Manage QMediaPlayers, QMediaPlaylist
- Handle Play/Pause/Prev/Next
- Expose state to QML by Q_PROPERTY
- Receive command from QML by Q_INVOKABLE
*/         
//================================================================================================

#ifndef __MUSICCONTROLLER_H__
#define __MUSICCONTROLLER_H__

#include <QObject>
#include <QMediaPlayer>
#include <QUrl>
#include <QList>
#include <QAudioOutput>             // Qt6 need to add audio output.
#include "../models/SongModel.h"
#include "../services/SettingService.h"

class MusicController : public QObject
{
    Q_OBJECT    // Macro for activating MOC (Meta-Object Compiler).

// Q_PROPERTY allows QML read and follow the value when that value changes.
    Q_PROPERTY( bool            isPlaying           READ    isPlaying           NOTIFY      isPlayingChanged )
    Q_PROPERTY( int             position            READ    position            NOTIFY      positionChanged )
    Q_PROPERTY( int             duration            READ    duration            NOTIFY      durationChanged )
    Q_PROPERTY( QString         currentTitle        READ    currentTitle        NOTIFY      currentSongChanged )
    Q_PROPERTY( QString         currentArtist       READ    currentArtist       NOTIFY      currentSongChanged )
    Q_PROPERTY( QString         currentCoverArt     READ    currentCoverArt     NOTIFY      currentSongChanged )
    Q_PROPERTY( SongModel*      songModel           READ    songModel           CONSTANT )
    Q_PROPERTY( int             currentIndex        READ    currentIndex        NOTIFY      currentIndexChanged )
    Q_PROPERTY( QString         theme               READ    theme               WRITE       setTheme            NOTIFY      themeChanged )
    Q_PROPERTY( qreal           volume              READ    volume              WRITE       setVolume           NOTIFY      volumeChanged )
    Q_PROPERTY( RepeatMode      repeatMode          READ    repeatMode          WRITE       setRepeatMode       NOTIFY      repeatModeChanged )
    Q_PROPERTY( bool            shuffle             READ    shuffle             WRITE       setShuffle          NOTIFY      shuffleChanged )

public:
// Parent-Child Memory Manangement to manange the storage. And avoid the object being created implicitly.
    explicit MusicController(QObject *parent = nullptr);

// Field of the repeat mode.
    enum RepeatMode {
        NotRepeat = 0,
        RepeatAll,
        RepeatOne
    };

    Q_ENUM(RepeatMode)

// Setter
    void setTheme( const QString& value );
    void setVolume( qreal value );
    void setRepeatMode( RepeatMode mode );
    void setShuffle( bool status );

// Getter - allow QML to call to retrieve the value.
    bool isPlaying() const;
    int position() const;
    int duration() const;
    QString currentTitle() const;
    QString currentArtist() const;
    QString currentCoverArt() const;
    SongModel* songModel() const;
    int currentIndex() const;
    QString theme() const;
    qreal volume() const;
    RepeatMode repeatMode() const;
    bool shuffle() const;

// Q_INVOKABLE - QML callable.
    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void playOrPause();
    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void setPosition(int pos);                  // Use for Slider seek bar.
    Q_INVOKABLE void setIndex(int idx);                     // Click the song in the playlist.
    Q_INVOKABLE void addSong(const QString &filePath);      // Add song to the playlist.
    Q_INVOKABLE void loadSongs( const QList<Song> &songs);  // Load new playlist to the model.
    Q_INVOKABLE void scanMusicDirectory();                  // Scan music file from directories.

// Declare signals which notify when the properties change.
signals:
    void isPlayingChanged();
    void positionChanged();
    void durationChanged();
    void currentSongChanged();
    void currentIndexChanged();
    void themeChanged();
    void volumeChanged();
    void repeatModeChanged();
    void shuffleChanged();

private:
    void loadCurrentSong();                     // Load song by m_currentIndex.
    const Song& songAt(int index) const;
    void createShuffleOrder();

    QMediaPlayer*   m_player;                   // Music play engine.
    QAudioOutput*   m_audioOutput;              // Audio.
    int             m_currentIndex;             // Current song index.
    SongModel*      m_songModel;                // Song Model.
    SettingService* m_settingService;           // Setting Service.
    qreal           m_volume;                   // Volume.
    RepeatMode      m_repeatMode;               // Repeat Mode.
    bool            m_shuffle;                  // Shuffle status;
    QList<int>      m_shuffleOrder;             // Order of songs when shuffle ON.
    int             m_shuffleIndex;             // Element index in the shuffle order.
};

#endif  // __MUSICCONTROLLER_H__
