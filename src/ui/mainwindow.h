#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QButtonGroup>
#include <QtPrintSupport/QPrinter>
#include <geometry/SceneProperties.h>

#include "sceneWidget.h"
#include "geometry/toolinfo.h"
#include "geometry/document.h"

namespace Ui
{
    class MainWindow;
}


using std::function;

using namespace geometry;

class QSplitter;

class ActionMaker
        : public QObject
{
    Q_OBJECT

    function<void()> ptr;
public:
    ActionMaker(function<void()> ptr, QObject *parent);
    ~ActionMaker();

public slots:
    void toggled(bool checked);
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupSplitter();

    void showMaximized();
    
signals:
    void nowRegistered();

public slots:
    void toggle640x480();
    void toggle800x600();

    void onNewDocument();
    void onOpenDocument();
    void onSaveDocument();
    void onSaveAsDocument();
    void onOpenMRUDocument();

    void onPrintPreview();
    void onPrint();
    void onExport();
    bool printPage(QPrinter*);
    void onFindText();
    void onCheckPoints();

    void toggledSelect(bool);
    void toggledMove(bool);
    void toggledLine(bool);
    void toggledElement(bool);
    void toggledErase(bool);
    void updatePlane();
    void selectPlane();
    void updateHelper(QString);
    void toolChanged();
    void checkRegistered();

    void onEnterSerial();
    void onBuy();
    void onBlock();
    void onUnregister();
    void onAbout();
    void onAboutQt();

    void onTogglePropertyTab();
    void onToggleToolsTab();

    void updateUIEvent();
    void closeEvent(QCloseEvent *);

    void startNewDocument();
    int readCommandLine();
    bool analyzeAutoSavedData();

private:
    void addToolSet(class WidgetBoxTreeWidget*, geometry::ToolSet);
    bool openDocument();
    bool openDocument(QString, bool *busy=0, bool autoSave=false);
    bool openAutoSaveDocument(QString openAs);
    bool saveDocument();
    bool saveAsDocument();
    bool saveCurrentDocument();
    bool saveDocument(QString);
    bool saveIfModified();
    void connectSignals();
    void setupTreeWidget();
    void setupPropertyWidget();

    void updateTitle();
    void analyzeRegistration();
    void updateMRU(QString name = "");
    void blockIfNotRegistered(bool onStart=1);

private:
    QActionGroup* instrumentGroup;
    QActionGroup* planeGroup;
    bool          registered;
    Ui::MainWindow *ui;
    QSplitter     *splitter;
    geometry::Scene* scene_;

    QMap<geometry::PToolInfo, class InstrumentButton*> mapTool2Button;

    QStringList mostRecentlyUsed_;
};

#endif // MAINWINDOW_H
