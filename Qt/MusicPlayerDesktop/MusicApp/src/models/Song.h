//================================================================================================
/// @file   Song.h.
/// @brief  Struct to save data of the song.
/*
Note:
- Save data of the song include: title, artist, path, duration, cover art.
*/
//================================================================================================

#ifndef __SONG_H__
#define __SONG_H__

#include <QString>

struct Song {
    QString title;      // Title.
    QString artist;     // Artist.
    QString path;       // Absolute path.
    int     duration;   // Milliseconds.
    QString coverArt;   // Cover art path, "" if none.
};

#endif // __SONG_H__
