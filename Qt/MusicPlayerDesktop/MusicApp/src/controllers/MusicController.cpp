//========================================================================================================================.
/// @file   MusicController.cpp.
// Note:
// - Flow: QML -> C++ -> QMediaPlayer/QMediaPlaylist -> emit signal to C++ -> emit signal to QML -> QML update UI.
//========================================================================================================================.

#include <QFileInfo>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QRandomGenerator>
#include <algorithm>
#include "MusicController.h"

// Function---------------------------------------------------.
// Summary: Constructor.
// Note:    Initialize MediaPlayer, AudioOutput and connect signal-slot.
MusicController::MusicController(QObject *parent)
    : QObject(parent)
    , m_currentIndex(0), m_repeatMode(NotRepeat), m_shuffle(false)
{
// Initilize.
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_songModel = new SongModel(this);
    m_settingService = new SettingService(this);
    // m_currentIndex = 0;

// Add audio output to the player.
    m_player->setAudioOutput(m_audioOutput);

// Set volume from the value retrieved from the registry.
    m_volume = m_settingService->volume();
    m_audioOutput->setVolume(m_volume);

// // Restore the last song.
//     m_currentIndex = m_settingService->lastSongIndex();
//     emit currentIndexChanged();

// // Restore the last position.
//     qint64 pos = m_settingService->lastSongPosition();
//     m_player->setPosition(pos);

/*============ CONNECT SIGNAL TO SLOT ============*/

// When the position of song changes, update slider progress.
connect( m_player, &QMediaPlayer::positionChanged,
        this, &MusicController::positionChanged );

// When the duration of song changes (change another song), update the UI.
connect( m_player, &QMediaPlayer::durationChanged,
        this, &MusicController::durationChanged );

// When the state of song changes, update icon.
connect( m_player, &QMediaPlayer::playbackStateChanged,
        this, &MusicController::isPlayingChanged);

// // When the song changes, update title.
// connect( m_playlist, &QMediaPlaylist::currentIndexChanged,
//         this, &MusicController::currentSongChanged);

// Emit signal when song changed.
// Automatively switch to next song if this song is end.
connect(m_player, &QMediaPlayer::mediaStatusChanged,
        this, [this](QMediaPlayer::MediaStatus status) {
            // if (status == QMediaPlayer::LoadedMedia) {
            //     emit currentSongChanged();
            // }
            if (status == QMediaPlayer::EndOfMedia) {
                next();
            }
        });

// Emit signal when data of current song changed (metadata for current song is complete).
connect(m_songModel, &QAbstractItemModel::dataChanged,
        this,
        [this](const QModelIndex &topLeft,
               const QModelIndex &bottomRight)
{
    Q_UNUSED(bottomRight);

    if (topLeft.row() == m_currentIndex) {
        emit currentSongChanged();
    }
});

// Show error log.
connect(m_player, &QMediaPlayer::errorOccurred,
        this, [&](QMediaPlayer::Error e){
            qWarning() << "Error:" << e
               << "Status:" << m_player->mediaStatus()
               << "Source:" << m_player->source();
        });
}
/*================= SETTER =====================*/

// Function---------------------------------------------------.
// Summary: Set theme.
// Return : None.
void MusicController::setTheme( const QString& value )
{
    if( value == m_settingService->theme() ) {
        return;
    }
    
    m_settingService->setTheme( value );
    emit themeChanged();
}

// Function---------------------------------------------------.
// Summary: Set volume.
// Return : None.
void MusicController::setVolume( qreal value )
{
    if( qFuzzyCompare(m_volume, value) ) {
        return;
    }

    m_volume = std::clamp(value, 0.0, 1.0);     // clamp - Limit the value from 0.0 to 1.0.
    m_audioOutput->setVolume(m_volume);
    m_settingService->setVolume(m_volume);

    emit volumeChanged();
}

// Function---------------------------------------------------.
// Summary: Set repeat mode.
// Return : None.
void MusicController::setRepeatMode( RepeatMode mode )
{
    if(m_repeatMode == mode) {
        return;
    }

    m_repeatMode = mode;
    emit repeatModeChanged();
}

// Function---------------------------------------------------.
// Summary: Set shuffle status.
// Return : None.
void MusicController::setShuffle( bool status )
{
    if(m_shuffle == status) {
        return;
    }
    
    m_shuffle = status;
    if( true == m_shuffle) {
        createShuffleOrder();
    }

    emit shuffleChanged();
}

/*================= GETTER =====================*/

// Function---------------------------------------------------.
// Summary: Check the state of player.
// Return : True - Playing.
//          False - Pause.
bool MusicController::isPlaying() const
{
    return m_player->playbackState() == QMediaPlayer::PlayingState;
}

// Function---------------------------------------------------.
// Summary: Get position of player.
// Return : .
int MusicController::position() const
{
    return m_player->position();
}

// Function---------------------------------------------------.
// Summary: Get duration of player.
// Return : .
int MusicController::duration() const
{
    return m_player->duration();
}

// Function---------------------------------------------------.
// Summary: Get current title of player.
// Return : .
QString MusicController::currentTitle() const
{
    const auto &list = m_songModel->getSongList();

    if (list.isEmpty() || m_currentIndex < 0 || m_currentIndex >= list.size()) {
        return QString("No song");
    }

    return list.at(m_currentIndex).title;
}

// Function---------------------------------------------------.
// Summary: Get current artist of player.
// Return : .
QString MusicController::currentArtist() const
{
    const auto &list = m_songModel->getSongList();

    if (list.isEmpty() || m_currentIndex < 0 || m_currentIndex >= list.size()) {
        return QString("");
    }

    return list.at(m_currentIndex).artist;
}

// Function---------------------------------------------------.
// Summary: Get current cover art.
// Return : .
QString MusicController::currentCoverArt() const
{
    const auto &list = m_songModel->getSongList();

    if( list.isEmpty() || m_currentIndex < 0 || m_currentIndex >= list.size() ) {
        return QString();
    }
    
    return list.at(m_currentIndex).coverArt;
}

// Function---------------------------------------------------.
// Summary: Get m_songModel.
// Return : .
SongModel* MusicController::songModel() const
{
    return m_songModel;
}

// Function---------------------------------------------------.
// Summary: Get current index.
// Return : .
int MusicController::currentIndex() const
{
    return m_currentIndex;
}

// Function---------------------------------------------------.
// Summary: Get current index.
// Return : .
QString MusicController::theme() const
{
    return m_settingService->theme();
}

// Function---------------------------------------------------.
// Summary: Get volume.
// Return : 
qreal MusicController::volume() const
{
    return m_volume;
}

// Function---------------------------------------------------.
// Summary: Get volume.
// Return : 
MusicController::RepeatMode MusicController::repeatMode() const
{
    return m_repeatMode;
}

// Function---------------------------------------------------.
// Summary: Get shuffle status.
// Return : 
bool MusicController::shuffle() const
{
    return m_shuffle;
}

/*============ PRIVATE FUNCTION=============*/

// Function---------------------------------------------------.
// Summary: Load current song - when switching another song.
// Return : .
void MusicController::loadCurrentSong()
{
    const auto &list = m_songModel->getSongList();

    if (m_currentIndex < 0 || m_currentIndex >= list.size()) {
        return;
    }

    const Song &s = list.at(m_currentIndex);
    m_player->setSource(QUrl::fromLocalFile(s.path));
    m_player->setPosition(0);

    m_player->play();
    emit currentSongChanged();
}

/*============ FUNCTION - QML CALL =============*/

// Function---------------------------------------------------.
// Summary: Play song.
// Return : .
void MusicController::play()
{
    m_player->play();
}

// Function---------------------------------------------------.
// Summary: Pause song.
// Return : .
void MusicController::pause()
{
    m_player->pause();
}

// Function---------------------------------------------------.
// Summary: Toggle Play/Pause.
// Return : .
void MusicController::playOrPause()
{
    if( isPlaying() ) {
        pause();
    } else {
        play();
    }
}

// Function---------------------------------------------------.
// Summary: Switch to the next song.
// Return : .
void MusicController::next()
{
    const auto &list = m_songModel->getSongList();

    if( list.isEmpty()) {
        return;
    }

    switch (m_repeatMode) {
        case NotRepeat:
            if( true == m_shuffle ) {
                if( m_shuffleIndex + 1 >= m_shuffleOrder.size() ) {
                    pause();
                } else {
                    ++m_shuffleIndex;
                    setIndex(m_shuffleOrder.at(m_shuffleIndex));
                }
                return;
            } else {
                if( m_currentIndex + 1 == list.size() ) {
                    pause();
                    return;
                } else {
                    m_currentIndex = m_currentIndex + 1;
                }
            }
            break;

        case RepeatAll:
            if( true == m_shuffle ) {
                if( m_shuffleIndex + 1 >= m_shuffleOrder.size() ) {
                    createShuffleOrder();
                    m_shuffleIndex = 0;
                    setIndex(m_shuffleOrder.at(0));
                } else {
                    ++m_shuffleIndex;
                    setIndex(m_shuffleOrder.at(m_shuffleIndex));
                }
                return;
            } else {
                m_currentIndex = (m_currentIndex + 1) % list.size();
            }
            break;

        case RepeatOne:
            setPosition(0);
            play();
            return;

        default:
            return;
    }
    setIndex(m_currentIndex);
}

// Function---------------------------------------------------.
// Summary: Switch to the previous song.
// Return : .
void MusicController::previous()
{
    const auto &list = m_songModel->getSongList();

    if( list.isEmpty()) {
        return;
    }

    if( true == m_shuffle ) {
        if(m_shuffleOrder.empty()) {
            return;
        }

        if( 0 >= m_shuffleIndex || m_shuffleIndex >= m_shuffleOrder.size()) {
            m_shuffleIndex = m_shuffleOrder.size() - 1;
        } else {
            --m_shuffleIndex;
        }
        setIndex(m_shuffleOrder.at(m_shuffleIndex));
    } else {
        int index = (m_currentIndex - 1 + list.size()) % list.size();  // Play back.
        setIndex(index);
    }
}

// Function---------------------------------------------------.
// Summary: Switch to another song.
// Return : .
void MusicController::setIndex(int index)
{
    const auto &list = m_songModel->getSongList();

    if (index < 0 || index >= list.size()) {
        return;
    }

    m_currentIndex = index;
    loadCurrentSong();

    // Set last song to the registry.
    m_settingService->setLastSongIndex( index );

    emit currentIndexChanged();
}

// Function---------------------------------------------------.
// Summary: Set the volume.
// Return : .
// void MusicController::setVolume(double value)
// {
//     // Set volume value.
//     m_audioOutput->setVolume(value);
//     // Save volume value to the registry.
//     m_settingService->setVolume(value);
// }

// Function---------------------------------------------------.
// Summary: Set the beginning position player.
// Return : .
void MusicController::setPosition(int pos)
{
    m_player->setPosition(pos);
    // Set last song position to the registry.
    m_settingService->setLastSongPosition(pos);
}

// Function---------------------------------------------------.
// Summary: Switch to another song.
// Return : .
void MusicController::addSong(const QString &filePath)
{
    QList<Song> list = m_songModel->getSongList();

    Song s;
    s.path = filePath;
    s.title = QFileInfo(filePath).baseName();

    list.append(s);
    m_songModel->setSongs(list);

    // If this is the first song, load now.
    if (list.size() == 1) {
        setIndex(0);
    }
}

// Function---------------------------------------------------.
// Summary: Load new playlist to the model.
// Param:   songs   The playlist.
// Return : None.
// Note:    .
void MusicController::loadSongs( const QList<Song> &songs )
{
    // Set data to the model.
    m_songModel->setSongs(songs);

    // If there is a song, load the first song.
    if (!songs.isEmpty()) {
        m_currentIndex = 0;
        loadCurrentSong();
    }
}

// Function---------------------------------------------------.
// Summary: Scan music file from directories.
// Param  : None.
// Return : None.
// Note:    .
void MusicController::scanMusicDirectory()
{
    // Get standard path in the system.
    // Music Path: C:/Users/<user>/Music.
    // QString musicDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);

    // Dir: "C:/Users/<Project>/build-MyPlayer-Desktop_Qt_6_6_0-Release/music".
    QString musicDir = QCoreApplication::applicationDirPath() + "/music";

    m_songModel->loadFromDirectory(musicDir);

    // If there is a song, play the first song.
    if (!m_songModel->getSongList().isEmpty()) {
        int lastSong = m_settingService->lastSongIndex();
        if (lastSong < 0 || lastSong >= m_songModel->getSongList().size()) {
            lastSong = 0;
        }

        m_currentIndex = lastSong;
        loadCurrentSong();
        m_player->setPosition(m_settingService->lastSongPosition());

        emit currentIndexChanged();
        emit currentSongChanged();
    }
}

// Function---------------------------------------------------.
// Summary: Get song based on index.
// Param  : .
// Return : .
// Note:    .
const Song& MusicController::songAt(int index) const
{
    return m_songModel->getSongList().at(index);
}

// Function---------------------------------------------------.
// Summary: Create shuffle order.
// Param  : .
// Return : .
// Note:    .
void MusicController::createShuffleOrder()
{
    const auto &list = m_songModel->getSongList();
    m_shuffleOrder.clear();

    if (list.isEmpty()) {
        m_shuffleIndex = 0;
        return;
    }

    // Create list.
    m_shuffleOrder.reserve(list.size());
    for (int i = 0; i < list.size(); ++i) {
        m_shuffleOrder.append(i);
    }

    // Shuffle.
    auto *rng = QRandomGenerator::global();
    std::shuffle(m_shuffleOrder.begin(), m_shuffleOrder.end(),
                *QRandomGenerator::global());

    // Get the index of shuffle order from current song index.
    m_shuffleIndex = m_shuffleOrder.indexOf(m_currentIndex);

    if (m_shuffleIndex < 0) {
        m_shuffleIndex = 0;
        m_currentIndex = m_shuffleOrder.at(0);
    }
}

