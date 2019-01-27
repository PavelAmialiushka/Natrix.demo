#include <QApplication>
#include "mainwindow.h"

#include "version.h"

#include <QTimer>
#include <QDir>

int main(int argc, char *argv[])
{
    QFileInfo exefile(argv[0]);
    QDir dir = exefile.absoluteDir();

    QString prepend;
    if (dir.exists())
        prepend = dir.absolutePath() + "\\";

    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(prepend + ".");
    paths.append(prepend + "imageformats");
    paths.append(prepend + "platforms");
    paths.append(prepend + "sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

    QApplication a(argc, argv);

    QString version = VER_PRODUCTVERSION_STR;
    a.setApplicationVersion(version);

    MainWindow w;

    if (!w.readCommandLine())
        return 0;

    return a.exec();
}
