//================================================================================================
/// @file   FileScanner.h.
/// @brief  Scan folders to get song files.
//================================================================================================

#ifndef __FileScanner_H__
#define __FileScanner_H__

#include <QString>
#include <QStringList>

class FileScanner {
public:
    // Scan music file.
    static QStringList scanMusicFiles( const QString &dir );
};

#endif // __FileScanner_H__