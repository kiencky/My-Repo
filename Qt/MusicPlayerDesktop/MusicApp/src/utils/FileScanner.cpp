//========================================================================================================================.
/// @file   FileScanner.cpp.
// Note:
//========================================================================================================================.

#include <QDirIterator>
#include "FileScanner.h"


// Function---------------------------------------------------.
// Summary: Scan folders and get song file list.
// Note:    .
QStringList FileScanner::scanMusicFiles( const QString &dir )
{
    QStringList lst;

    // Create an iterator to scan each file.
    QDirIterator it(dir,
                    QStringList() << "*.mp3" << "*.wav" << "*.flac" << "*.m4a",
                    QDir::Files,
                    QDirIterator::Subdirectories);
    
    // Scan each file.
    while (it.hasNext()) {
        lst.append(it.next());      // Add file to the list.
    }

    return lst;
}