//================================================================================================
/// @file   SongModel.h.
/// @brief  Manage the song data.
//================================================================================================

#ifndef __SONGMODEL_H__
#define __SONGMODEL_H__

#include <QAbstractListModel>   // Base class so that QML can communicate with backend C++ data.
#include <QList>
#include "Song.h"

class SongModel : public QAbstractListModel
{
    Q_OBJECT
public:

    // Enum - field of the song.
    enum SongRoles {
        TitleRole    = Qt::UserRole + 1,    // 257.
        ArtistRole,
        PathRole,
        DurationRole,
        CoverArtRole,
        IndexRole
    };

    Q_ENUM(SongRoles)  // Register the enum so that Qt can recognize it.

    // Parent-Child Memory Manangement to manange the storage.
    explicit SongModel(QObject *parent = nullptr);

    // Get list of song.
    const QList<Song>& getSongList() const;

// Override these method when inheriting QAbstractListModel.
    // The number of data row.
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // The data of each row.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Mapping role name for QML.
    QHash<int, QByteArray> roleNames() const override;

// Load new data to the model.
    void setSongs(const QList<Song> &songs);

// Load song files from directories.
    Q_INVOKABLE void loadFromDirectory( const QString &path );

// Get data of song in the index.

private:
    QList<Song> m_songs;  // Store playlist data.

};

#endif // __SONGMODEL_H__
