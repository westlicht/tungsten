#include <embree/include/embree.h>
#include <QDesktopWidget>
#include <QApplication>
#include <QGLWidget>
#include <QDir>

#include "thread/ThreadUtils.hpp"

#include "io/FileUtils.hpp"

#include "RenderWindow.hpp"
#include "MainWindow.hpp"

#ifdef OPENVDB_AVAILABLE
#include <openvdb/openvdb.h>
#endif

using namespace Tungsten;

int main(int argc, char *argv[])
{
    int threadCount = max(ThreadUtils::idealThreadCount() - 1, 1u);
    ThreadUtils::startThreads(threadCount);

    embree::rtcInit();
    embree::rtcStartThreads(threadCount);

#ifdef OPENVDB_AVAILABLE
        openvdb::initialize();
#endif

    // Use OpenGL 3.2 core-profile
    // Note: This is due to a limitation in MacOSX, where the default format
    // needs to be set before creating an instance of QOpenGLWidget.
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setMajorVersion(3);
    surfaceFormat.setMinorVersion(2);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    surfaceFormat.setAlphaBufferSize(8);
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setSamples(4);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    QApplication app(argc, argv);

    QDir::setCurrent(QString::fromStdString(FileUtils::getExecutablePath().parent().nativeSeparators().asString()));
    Path path = FileUtils::getExecutablePath().parent()/"data/editor/style/style.qss";
    app.setStyleSheet(QString::fromStdString(FileUtils::loadText(path)));

    QDesktopWidget desktop;
    QRect windowSize(desktop.screenGeometry(desktop.primaryScreen()));
    windowSize.adjust(100, 100, -100, -100);

    MainWindow mainWindow;
    mainWindow.setWindowTitle("Tungsten Scene Editor");
    mainWindow.setGeometry(windowSize);

    mainWindow.show();

    Path testScenePath = FileUtils::getExecutablePath().parent()/"data/materialtest/materialtest.json";
    if (argc > 1)
        mainWindow.openScene(QString(argv[1]));
    else if (FileUtils::exists(testScenePath))
        mainWindow.openScene(QString(testScenePath.asString().c_str()));

    try {
        return app.exec();
    } catch (std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
