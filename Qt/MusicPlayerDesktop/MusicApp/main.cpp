//=================================================================================.
/// @file   main.cpp.
//=================================================================================.
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "src/controllers/MusicController.h"
#include <QIcon>
#include <QQuickStyle>

int main(int argc, char *argv[])
{
    // Allow customize styles.
    QQuickStyle::setStyle("Fusion");

    QGuiApplication app(argc, argv);

    // Set icon app.
    app.setWindowIcon(QIcon(":/assets/icons/app.png"));

    // Create instance.
    // MusicController c_MusicController;

    // Use Uncreatable because SongModel is created by MusicController.  
    qmlRegisterUncreatableType<SongModel>(
                                    "MusicApp",     // Module name used in QML: import MusicApp 1.0.
                                    1, 0,           // Version 1.0.
                                    "SongModel",    // Type name used in QML.
                                    "SongModel is created by MusicController"  // Inform error if initilize by new.
    );

    // Register Singleton instance and expose it to QML.
    qmlRegisterSingletonType<MusicController>(
        "MusicApp", 1, 0, "MusicController",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return new MusicController();
        }
    );

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("MusicApp", "Main");

    return app.exec();
}
