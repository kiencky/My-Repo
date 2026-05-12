cmake_minimum_required(VERSION 3.16)

project(MusicApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS 
    Quick
    Multimedia
    QuickControls2
)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appMusicApp
    main.cpp
)

# Recursively scan all .h .cpp files in "src" directory.
file(GLOB_RECURSE APP_SOURCES
    src/*.cpp
    src/*.h
)

# Add singleton files.
set_source_files_properties(
    qml/styles/Theme.qml
    qml/styles/Colors.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(appMusicApp
    URI MusicApp
    QML_FILES
        qml/Main.qml

        qml/components/NowPlaying.qml
        qml/components/PlayerControls.qml
        qml/components/Playlist.qml
        qml/components/SongItem.qml

        qml/styles/Theme.qml
        qml/styles/Colors.qml
    SOURCES
        ${APP_SOURCES}
)

# Cre: https://www.flaticon.com
qt_add_resources(appMusicApp "assets"
    PREFIX "/"
    FILES
        assets/cover/disc.gif
        assets/icons/app.png
        assets/icons/cd.gif
        assets/icons/mute.png
        assets/icons/next.png
        assets/icons/notRepeat.png
        assets/icons/pause.png
        assets/icons/play.png
        assets/icons/prev.png
        assets/icons/repeatAll.png
        assets/icons/repeatOne.png
        assets/icons/shuffle.png
        assets/icons/sound.png
)

# Create music path
set(MUSIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/music)

# Copy MUSIC_DIR to directory that include execute file.
add_custom_command(TARGET appMusicApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${MUSIC_DIR}"
        "$<TARGET_FILE_DIR:appMusicApp>/music"
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appMusicApp PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appMusicApp
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Add icon file for exe.
target_sources(appMusicApp PRIVATE app.rc)

target_link_libraries(appMusicApp
    PRIVATE
        Qt6::Quick
        Qt6::Multimedia
        Qt6::QuickControls2
)

include(GNUInstallDirs)
install(TARGETS appMusicApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Add music folder when installing.
install(DIRECTORY ${MUSIC_DIR}/
        DESTINATION music)
