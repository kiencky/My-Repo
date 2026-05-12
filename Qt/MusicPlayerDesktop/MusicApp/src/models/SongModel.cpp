//========================================================================================================================.
/// @file   SongModel.cpp.
// Note:
// - Implement song model.
//========================================================================================================================.

#include "SongModel.h"
#include "../utils/FileScanner.h"
#include <QFileInfo>
#include <QMediaPlayer>
#include <QStandardPaths>
#include <QMediaMetaData>
#include <QImage>

// Function---------------------------------------------------.
// Summary: Constructor.
// Note:    .
SongModel::SongModel(QObject *parent)
        : QAbstractListModel(parent)
{}

// Function---------------------------------------------------.
// Summary: Constructor.
// Note:    .
const QList<Song>& SongModel::getSongList() const
{
    return m_songs;
}

// Function---------------------------------------------------.
// Summary: Return the number of data row.
// Param:   parent      The address of an item in the model.
// Note:    .
int SongModel::rowCount(const QModelIndex &parent) const
{
    // Check if list is a tree or a flat list, if it's a tree, return 0.
    if(parent.isValid()) {
        return 0;
    }

    // Return the number of row.
    return m_songs.size();
}

// Function---------------------------------------------------.
// Summary: Return the data of each row.
// Return:  QVariant    The container that can return the appropriate data type.
// Note:    .
QVariant SongModel::data(const QModelIndex &index, int role ) const
{
    // Check if list is a tree, and the row number is out of the number of song.
    if(!index.isValid() || index.row() >= m_songs.size()) {
        return QVariant();      // return empty.
    }

    // Reference to the song at index row.
    const Song& song = m_songs.at(index.row());

    switch(role) {
        case TitleRole:     return song.title;
        case ArtistRole:    return song.artist;
        case PathRole:      return song.path;
        case DurationRole:  return song.duration;
        case CoverArtRole:  return song.coverArt;
        case IndexRole:     return index.row();
        default:            return QVariant();  // return empty.
    }
}

// Function---------------------------------------------------.
// Summary: Mapping role name for QML.
// Param:   None.
// Return:  QHash<int, QByteArray>  Mapping from role ID to role name.
// Note:    The mapping table between role ID (int) and role name (QByteArray).
//          QML can access data by name (model.title) instead of using role number (model[257]).
QHash<int, QByteArray> SongModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[TitleRole]    = "title";      // QML: model.title.
    roles[ArtistRole]   = "artist";     // QML: model.artist.
    roles[PathRole]     = "path";       // QML: model.path.
    roles[DurationRole] = "duration";   // QML: model.duration.
    roles[CoverArtRole] = "coverArt";   // QML: model.coverArt.
    roles[IndexRole]    = "index";      // QML: modle.index.

    return roles;
}

// Function---------------------------------------------------.
// Summary: Load new data to the model.
// Param:   .
// Return:  None.
// Note:    beginResetModel() and endResetModel are required to notify QML that there are changes.
void SongModel::setSongs(const QList<Song> &songs)
{
    beginResetModel();      // Notify that is about to change.
    m_songs = songs;
    endResetModel();        // Notify that the change is done.
}

// Function---------------------------------------------------.
// Summary: Load song file from directories.
// Param:   .
// Return:  None.
// Note:    .
void SongModel::loadFromDirectory( const QString &path )
{
    beginResetModel();
    m_songs.clear();

    // Scan music file.
    QStringList files = FileScanner::scanMusicFiles(path);

    // for (const QString &file : files) {
    //     Song s;
    //     // Get file path.
    //     s.path = file;

    //     // Create file info object from file path.
    //     QFileInfo info(file);
    //     s.title = info.baseName();

    //     int index = m_songs.size();
    //     m_songs.append(s);

    //     // Create temp QMediaPlayer to get metadata MP4/M4A.
    //     QMediaPlayer *tmp = new QMediaPlayer(this);
    //     tmp->setSource(QUrl::fromLocalFile(file));

    //     // Active backend to read metadata.
    //     tmp->play();
    //     tmp->pause();

    //     QObject::connect(tmp, &QMediaPlayer::metaDataChanged, this,
    //         [this, tmp, index]() {

    //         auto md = tmp->metaData();

    //         m_songs[index].title = md.stringValue(QMediaMetaData::Title);
    //         m_songs[index].artist = md.stringValue(QMediaMetaData::Author);
    //         m_songs[index].duration = md.value(QMediaMetaData::Duration).toLongLong();

    //         // m_songs[index].coverArt = md.value(QMediaMetaData::CoverArtImage).value<QImage>();
    //         // Create an QImage object -> Save to a file png -> Assign the image path to coverArt. 
    //         QImage img = md.value(QMediaMetaData::CoverArtImage).value<QImage>();
    //         QString coverPath = QString("%1/cover_%2.png")
    //                 .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
    //                 .arg(index);
    //         img.save(coverPath);
    //         m_songs[index].coverArt = coverPath;

    //         emit dataChanged(this->index(index), this->index(index));

    //         // Delete after all signals have been completely processed.
    //         tmp->deleteLater();
    //     });
    // }

    for (const QString &file : files) {
        Song s;
        // Create file info object from file path.
        QFileInfo info(file);

        s.path = file;
        s.title = info.baseName();

        int index = m_songs.size();
        m_songs.append(s);

        QMediaPlayer *tmp = new QMediaPlayer(this);

        // Get data from metadata.
        QObject::connect(tmp, &QMediaPlayer::mediaStatusChanged, this,
            [this, tmp, index](QMediaPlayer::MediaStatus status) {

            if (status != QMediaPlayer::LoadedMedia)
                return;

            auto md = tmp->metaData();

            qDebug() << "=========== METADATA ===========";

            for (auto key : md.keys()) {
                qDebug()
                    << key
                    << ":"
                    << md.value(key);
            }
            
            // Title.
            m_songs[index].title    = md.stringValue(QMediaMetaData::Title);

            // Artist.
            QStringList artists     = md.value(QMediaMetaData::ContributingArtist).toStringList();
            m_songs[index].artist   = artists.join(", ");

            // Duration.
            m_songs[index].duration = md.value(QMediaMetaData::Duration).toLongLong();

            // Create an QImage object -> Save to a file png -> Assign the image path to coverArt.
            QImage img = md.value(QMediaMetaData::ThumbnailImage).value<QImage>();
            if (!img.isNull()) {
                QString coverPath = QString("%1/cover_%2.png")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                        .arg(index);
                img.save(coverPath);
            
                m_songs[index].coverArt = QUrl::fromLocalFile(coverPath).toString();
            }

            emit dataChanged(this->index(index), this->index(index));

            // Delete after all signals have been completely processed.
            tmp->deleteLater();
        });

        tmp->setSource(QUrl::fromLocalFile(file));
    }

    endResetModel();
}
