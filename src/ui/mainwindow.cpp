#include "mainwindow.h"
#include "geometry/manipulator.h"
#include "geometry/DocumentAutoSaver.h"
#include "WidgetBoxTreeWidget.h"
#include "instrumentbutton.h"

#include "../../bin/ui/ui_mainwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QDebug>

#include <QSettings>

#include <QDesktopServices>
#include <QUrl>

#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QtWidgetsDepends>
#include <QSplitter>

#include "exportDialog.h"
#include "registerDialog.h"
#include "aboutDialog.h"
#include "blockDialog.h"
#include "newDialog.h"

#include "sceneWidgetDrawer.h"
#include "AutoRestoreManagerDialog.h"
#include "FindDialog.h"
#include "CheckPointsDialog.h"
#include "propertyTreeWidget.h"

#pragma GCC diagnostic push
#include "secman/securityCheckCRC.h"
#include "secman/securityStorage.h"
#include "secman/securityCodes.h"

#include <geometry/SingletonFileWindow.h>

#pragma GCC diagnostic pop


ActionMaker::ActionMaker(function<void()> ptr, QObject *parent)
    : QObject(parent)
    , ptr(ptr)
{
}

ActionMaker::~ActionMaker()
{
}

void ActionMaker::toggled(bool checked)
{
    if (checked)
        ptr();
}

namespace
{
    QString& makeNativePath(QString& path)
    {
        return path.replace("/", "\\");
    }
}

void MainWindow::addToolSet(WidgetBoxTreeWidget* treeWidget,
                            geometry::ToolSet set)
{
    QString name = set.name;

    // create content and fill it with something
    QWidget *content = new QWidget();

    QGridLayout *layout = new QGridLayout(content);
    layout->setSpacing(2);
    layout->setMargin(2);

    const int columnCount = 4;
    layout->setColumnStretch(columnCount, 1);

    int instrumentMenuIndex = 3;
    Q_ASSERT(menuBar()->actions().size()>instrumentMenuIndex);

    QMenu *menu = menuBar()->actions()[instrumentMenuIndex]->menu();
    if (menu->actions().size() > 12 /*magic*/)
    {
        ++instrumentMenuIndex;
        menu = menuBar()->actions()[instrumentMenuIndex]->menu();
    }

    if (menu->actions().size())
        menu->addSeparator();

    int index=0;
    foreach(PToolInfo info, set.tools)
    {
        InstrumentButton * button = new InstrumentButton(content);
        QString iconName = QString(":/draw-%1.png").arg(info->regName.toLower());

        QAction* action = new QAction(this);
        action->setActionGroup(instrumentGroup);
        action->setCheckable(true);
        action->setShortcut(info->shortCut);
        action->setIcon(QIcon(iconName));
        action->setText(info->name);

        menu->addAction( action );

        button->setIcon(QIcon(iconName));

        button->setIconSize(QSize(32,32));
        button->setText(info->name);
        button->setToolTip(info->name);
        button->setCheckable(true);
        button->setDefaultAction(action);

        connect(button, SIGNAL(toggled(bool)), action, SLOT(setChecked(bool)));

        ActionMaker* maker = new ActionMaker([=]()
        {
            scene_->manipulator()->setToolInfo( info );
        },
        this);
        connect(button, SIGNAL(toggled(bool)), maker, SLOT(toggled(bool)));

        layout->addWidget(button, index/columnCount, index%columnCount);
        mapTool2Button[info] = button;
        ++index;
    }
    // add to tree
    QTreeWidgetItem * treeItem = new QTreeWidgetItem(treeWidget);
    treeItem->setFlags(Qt::ItemIsEnabled);
    treeItem->setText(0, name);

    QTreeWidgetItem * embed_item = new QTreeWidgetItem(treeItem);
    embed_item->setFlags(Qt::ItemIsEnabled);

    treeWidget->setItemWidget(embed_item, 0, content);
    embed_item->setExpanded(true);
}

void MainWindow::toolChanged()
{
    PToolInfo tool = scene_->manipulator()->toolInfo();
    auto* button = mapTool2Button[tool];
    if (button) button->setChecked(true);
}

void MainWindow::setupSplitter()
{
    splitter = new QSplitter(ui->centralPlacement);

    // разбираем старый layout
    delete ui->centralPlacement->layout();

    // создаём новый
    auto* l = new QVBoxLayout;
    l->addWidget(splitter);
    ui->centralPlacement->setLayout(l);

    splitter->setOrientation(Qt::Horizontal);
    splitter->setChildrenCollapsible(false);

    splitter->addWidget(ui->zoneA_2);
    splitter->addWidget(ui->zoneB_2);
    splitter->addWidget(ui->zoneC_2);

    splitter->handle(1)->setEnabled(false);
    splitter->handle(2)->setEnabled(false);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 0);

    QSettings s;
    if (!s.value("ToolsWindowVisible", 1).toInt())
        ui->zoneA_2->hide();

    if (!s.value("PropertyWindowVisible", 1).toInt())
        ui->zoneC_2->hide();
}

void MainWindow::showMaximized()
{
    if (!isVisible())
    {
        showNormal();
        QTimer::singleShot(10, [&]()
        {
            showMaximized();
        });
        return;
    }

    QMainWindow::showMaximized();
    ui->sceneWidget->updateSceneScale();
}

void MainWindow::setupTreeWidget()
{
    // Tool Tree Widget
    WidgetBoxTreeWidget *treeWidget = new WidgetBoxTreeWidget(ui->zoneA_2);
    treeWidget->setBackgroundRole( QPalette::Button );

    // копируем политику размеров
//    treeWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
//                                          QSizePolicy::Preferred));
    treeWidget->setSizePolicy(ui->toolsTab->sizePolicy());
    delete ui->toolsTab;

    // подключаемся к layout
    auto layout = ui->zoneA_2->layout();
    layout->addWidget(treeWidget);

    // установка инструментов
    instrumentGroup = new QActionGroup(this);
    auto sets = ToolSetFactory::inst().sets();
    foreach(ToolSet set, sets)
        addToolSet(treeWidget, set);
    treeWidget->setItemExpanded(treeWidget->topLevelItem(1), true);
    treeWidget->expandAll();

    // регистрируем реакцию на изменение инструмента
    connect(scene_->manipulator(), SIGNAL(toolChanged()),
            this, SLOT(toolChanged()));
}

void MainWindow::setupPropertyWidget()
{
    // Property Layout
    auto *browserWidget = new PropertyTreeWidget(ui->propertyTabFrame_2, scene_);

    // копируем политику размеров
    browserWidget->setSizePolicy(ui->propertyTab->sizePolicy());
    delete ui->propertyTab;

    // добавляем виджет к лейауту
    auto layout = ui->propertyTabFrame_2->layout();
    layout->addWidget(browserWidget);

    setBackgroundRole( QPalette::Button );
}

void MainWindow::onFindText()
{
    FindDialog dialog(scene_);
    dialog.exec();
}

void MainWindow::onCheckPoints()
{
    CheckPointsDialog dialog(scene_);
    dialog.exec();
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QCoreApplication::setOrganizationName("NatrixLabs");
    QCoreApplication::setOrganizationDomain("NatrixLabs.ru");
    QCoreApplication::setApplicationName("Natrix");

    // тип здесь не имеет значения, сцена обновляется далее
    scene_ = new geometry::Scene(1);

    // анализ данны по регистрации
    analyzeRegistration();

    // восстанаваливаем список использованных файлов
    foreach(auto m, QSettings().value("mru").toStringList())
    {
        auto path = makeNativePath(m);
        if (!mostRecentlyUsed_.contains(path))
            mostRecentlyUsed_ << path;
    }

    updateMRU();

    // создаем сигналы и контролы
    connectSignals();

    setupSplitter();
    setupTreeWidget();
    setupPropertyWidget();

    // ПОСЛЕ ПОДКЛЮЧЕНИЯ СИГНАЛОВ

    ui->sceneWidget->setDocument( scene_->document() );
    scene_->manipulator()->setToolInfo( ToolSetFactory::inst().toolLine() );

    // обновление заголовка
    QTimer::singleShot(200, [&]()
    {
        updateTitle();
    });
    showMaximized();
}

void MainWindow::connectSignals()
{
    connect(ui->actionNew, SIGNAL(triggered()), SLOT(onNewDocument()));
    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(onOpenDocument()));
    connect(ui->actionSave, SIGNAL(triggered()), SLOT(onSaveDocument()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), SLOT(onSaveAsDocument()));
    connect(ui->action1_MRU, SIGNAL(triggered()), this, SLOT(onOpenMRUDocument()));
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(onPrint()));
    connect(ui->actionPreview, SIGNAL(triggered()), this, SLOT(onPrintPreview()));
    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(onExport()));
    connect(ui->actionFindText, SIGNAL(triggered()), this, SLOT(onFindText()));
    connect(ui->actionCheckPoints, SIGNAL(triggered()), this, SLOT(onCheckPoints()));

    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));

    connect(ui->actionUndo, SIGNAL(triggered()), ui->sceneWidget, SLOT(undoEvent()));
    connect(ui->actionRedo, SIGNAL(triggered()), ui->sceneWidget, SLOT(redoEvent()));
    connect(ui->actionCopy, SIGNAL(triggered()), ui->sceneWidget, SLOT(onCopy()));
    connect(ui->actionCut, SIGNAL(triggered()), ui->sceneWidget, SLOT(onCut()));
    connect(ui->actionPaste, SIGNAL(triggered()), ui->sceneWidget, SLOT(onPaste()));
    connect(ui->actionDelete, SIGNAL(triggered()), ui->sceneWidget, SLOT(onDelete()));
    connect(ui->actionSelectAll, SIGNAL(triggered()), ui->sceneWidget, SLOT(onSelectAll()));
    connect(ui->actionSelectNone, SIGNAL(triggered()), ui->sceneWidget, SLOT(onSelectNone()));

    connect(ui->sceneWidget, SIGNAL(updateHelper(QString)), this, SLOT(updateHelper(QString)));

    connect(ui->action_640_480, SIGNAL(triggered()), SLOT(toggle640x480()));
    connect(ui->action_800_600, SIGNAL(triggered()), SLOT(toggle800x600()));
    connect(ui->actionBlock, SIGNAL(triggered()), SLOT(onBlock()));
    connect(ui->actionRegister, SIGNAL(triggered()), SLOT(onEnterSerial()));
    connect(ui->actionUnregister, SIGNAL(triggered()), SLOT(onUnregister()));

    connect(ui->action_Qt, SIGNAL(triggered()), SLOT(onAboutQt()));
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(onAbout()));

    ui->actionPropertyTab->setChecked(true);
    ui->actionToolsTab->setChecked(true);
    connect(ui->actionPropertyTab, SIGNAL(changed()), SLOT(onTogglePropertyTab()));
    connect(ui->actionToolsTab, SIGNAL(changed()), SLOT(onToggleToolsTab()));

    planeGroup = new QActionGroup(this);
    ui->actionNoPlane->setActionGroup(planeGroup);
    ui->actionPlaneA->setActionGroup(planeGroup);
    ui->actionPlaneB->setActionGroup(planeGroup);
    ui->actionPlaneC->setActionGroup(planeGroup);
    connect(ui->actionPlaneA, SIGNAL(toggled(bool)), this, SLOT(selectPlane()));
    connect(ui->actionPlaneB, SIGNAL(toggled(bool)), this, SLOT(selectPlane()));
    connect(ui->actionPlaneC, SIGNAL(toggled(bool)), this, SLOT(selectPlane()));
    connect(ui->actionNoPlane, SIGNAL(toggled(bool)), this, SLOT(selectPlane()));

    connect(scene_->manipulator(), SIGNAL(updatePlaneMode()), this, SLOT(updatePlane()));
    connect(scene_, SIGNAL(updateUI()), this, SLOT(updateUIEvent()));

    auto t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(updateUIEvent()));
    t->start(100);

    updatePlane();
}

void MainWindow::startNewDocument()
{
    int docType = QSettings().value("plane_type", 1).toInt();
    scene_->document()->newDocument((DocumentType)docType);
}

int MainWindow::readCommandLine()
{
    auto args = QCoreApplication::arguments();

    QCommandLineParser parser;
    auto load_autosave = QCommandLineOption("load-autosave");
    parser.addOption(load_autosave);

    auto autosave_manager = QCommandLineOption("autorestore-manager");
    parser.addOption(autosave_manager);

    parser.parse(args);
    args = parser.positionalArguments();

    if (parser.isSet(autosave_manager))
    {
        Q_ASSERT(args.size());
        if (!args.size()) return false;

        QFile file(args.first());
        file.open(QIODevice::ReadOnly);
        auto data = file.readAll();
        auto fileNames = QString::fromUtf8(data).split("\n");

        auto dialog = new AutoRestoreManagerDialog(fileNames, 0);
        dialog->exec();
        return false;
    }

    if (parser.isSet(load_autosave))
    {
        Q_ASSERT(args.size());
        if (!args.size()) return false;

        // пытаемся открыть файл из коммандной строки
        if (openAutoSaveDocument(args.first()))
            return true;

        return false;
    }


    // нормальная загрузка
    if (analyzeAutoSavedData())
        return false;

    if (args.size())
    {
        bool busy = false;
        if (openDocument(args.first(), &busy))
            return true;

        // закрываем программу
        return false;
    }

    startNewDocument();
    return true;
}

bool MainWindow::analyzeAutoSavedData()
{
    QStringList records = geometry::DocumentAutoSaver::takeUnsavedRecords();
    if (records.isEmpty())
        return false;

    QTemporaryFile file;
    file.open();
    file.setAutoRemove(false);
    foreach(QString fileName, records)    
        file.write(fileName.toUtf8() + "\n");
    file.close();

    QStringList args;
    args << "--autorestore-manager"
         << file.fileName();

    auto process = new QProcess;
    process->setProcessEnvironment(
                QProcessEnvironment::systemEnvironment());
    auto exeFile = QCoreApplication::arguments().first();
    process->start(exeFile, args);

    return true;
}

void MainWindow::analyzeRegistration()
{
    // блокировка если незарегистрирован
    auto info = Security::retrieveRegistrationInfo();
    registered = Security::checkCode(info.serial);

    // добавляем динамические пункты меню
    QMenu* helpMenu =  menuBar()->actions().last()->menu();
    QList<QAction*> actions;
#ifndef NDEBUG
    actions << ui->action_640_480;
    actions << ui->action_800_600;
    actions << ui->actionBlock;
#endif
    if (!registered)
    {
        actions <<  ui->actionRegister;
#ifndef NDEBUG
    }
    else
    {
        actions << ui->actionUnregister;
#endif
    }
    QAction* firstMenuItem = helpMenu->actions().first();
    firstMenuItem = helpMenu->insertSeparator(firstMenuItem);

    // insertActions работает неверно для списка больше 2х
    // поэтому вставляем вручную
    while (actions.size())
    {
        helpMenu->insertAction(firstMenuItem, actions.last());
        firstMenuItem = actions.takeLast();
    }

    blockIfNotRegistered();
}

void MainWindow::blockIfNotRegistered(bool onStart)
{
    if (!onStart)
    {
        if (!Security::checkCRC<1>())
        {
            QMessageBox box;
            box.setText(QObject::trUtf8("Ошибка контрольной суммы файла. "
                                        "Приложение будет закрыто."));
            box.exec();
            qApp->exit(1);
        }
    }

    // проверка регистрации
    if (!registered
            && BlockDialog::shallShowDialog(onStart))
    {
        onBlock();
    }
}

void MainWindow::onBlock()
{
    BlockDialog dialog;
    connect(&dialog, SIGNAL(askToBuy()), SLOT(onBuy()));
    connect(&dialog, SIGNAL(askToEnterSerial()), SLOT(onEnterSerial()));
    connect(this, SIGNAL(nowRegistered()), &dialog, SLOT(nowRegistered()));

    dialog.exec();

    // повторяем проверку, вдруг был введен код
    checkRegistered();
}

void MainWindow::updateHelper(QString text)
{
    ui->textPanel->setText(text);
}

void MainWindow::toggledSelect(bool)
{
    scene_->manipulator()->setToolInfo(
        ToolSetFactory::inst().toolSelect());
}

void MainWindow::toggledMove(bool)
{
    scene_->manipulator()->setToolInfo(
        ToolSetFactory::inst().toolMove());
}

void MainWindow::toggledLine(bool)
{
    scene_->manipulator()->setToolInfo(
        ToolSetFactory::inst().toolLine());
}

void MainWindow::toggledErase(bool)
{
    scene_->manipulator()->setToolInfo(
        ToolSetFactory::inst().toolErase());
}

void MainWindow::toggledElement(bool)
{
    scene_->manipulator()->setToolInfo(
        ToolSetFactory::inst().toolElement());
}

void MainWindow::updatePlane()
{
    int plane = scene_->manipulator()->planeMode();
    switch(plane)
    {
    case 0: ui->actionNoPlane->setChecked(1); break;
    case 1: ui->actionPlaneA->setChecked(1); break;
    case 2: ui->actionPlaneB->setChecked(1); break;
    case 3: ui->actionPlaneC->setChecked(1); break;
    }
}

void MainWindow::selectPlane()
{
    scene_->manipulator()->setPlaneMode(
                ui->actionPlaneA->isChecked() ? 1 :
                ui->actionPlaneB->isChecked() ? 2 :
                ui->actionPlaneC->isChecked() ? 3 : 0);

    // WORKAROUND: сбрасываем ожидание шифта, чтобы не регистрировалось его отпускание
    ui->sceneWidget->dropWaitingShift();
}

void MainWindow::updateTitle()
{
    QString ver = QApplication::instance()->applicationVersion();
    QString appName = QApplication::instance()->applicationName();

    QString fn = scene_->document()->fileName();
    fn = fn.split('\\').last();

    if (scene_->document()->autoRestored())
        fn += " (восстановлен)";

    setWindowTitle(QString("%1%2 - %3 %4 %5")
                   .arg(scene_->document()->modified() ? "*" : "")
                   .arg((fn.isEmpty() ? trUtf8("Новый") : fn))
                   .arg(appName)
                   .arg(ver)
                   .arg(registered ? "" : " - " + trUtf8("ознакомительная версия"))
                   );
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();

    if (!scene_->document()->modified())
        return;

    QMessageBox msg;
    msg.setText(trUtf8("Сохранить изменения в файле?"));
    msg.setWindowTitle( QApplication::instance()->applicationName() );
    msg.setInformativeText(scene_->document()->fileName());
    msg.setStandardButtons( QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
    msg.setDefaultButton( QMessageBox::Save );
    msg.setIcon( QMessageBox::Warning );
    int result = msg.exec();
    if (result == QMessageBox::Yes)
    {
        if (!saveDocument())
            event->ignore();
    } else if (result == QMessageBox::No)
    {
        return;
    } else {
        // Cancel
        event->ignore();
    }
}

void MainWindow::onNewDocument()
{
    if (!saveIfModified())
        return;

    NewDialog dialog;
    int r = dialog.exec();
    if (!r) return;

    scene_->document()->newDocument((DocumentType)r);
    updateTitle();
}

void MainWindow::onOpenDocument()
{
    openDocument();
}

bool MainWindow::openDocument()
{
    if (!saveIfModified())
        return false;

    QSettings sett;
    QString defaultPath = "c:\\";
    defaultPath = sett.value("default_path", defaultPath).toString();

    QString fileName = QFileDialog::getOpenFileName(this,
        trUtf8("Открыть файл"), // caption
        defaultPath, trUtf8("Natrix files (*skt *.sktx)"));

    if (fileName.isEmpty()) return false;
    return openDocument(fileName);
}

bool MainWindow::saveIfModified()
{
    if (scene_->document()->modified())
    {
        QMessageBox msg;
        msg.setText(trUtf8("Сохранить изменения в файле?"));
        msg.setWindowTitle( QApplication::instance()->applicationName() );
        msg.setInformativeText(scene_->document()->fileName());
        msg.setStandardButtons( QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
        msg.setDefaultButton( QMessageBox::Save );
        msg.setIcon( QMessageBox::Warning );
        int result = msg.exec();
        if (result == QMessageBox::Yes)
        {
            if (saveDocument())
                return true;
            else
                return false;
        } else if (result == QMessageBox::No)
            return true;
        return false;
    }

    return true;
}

bool MainWindow::openDocument(QString fileName, bool *busy, bool autoSave)
{
    QSharedPointer<ExclusiveFile> file(new ExclusiveFile{fileName});
    SingletonFileWindow::connect(file.data(), this);

    if (!file->exists())
    {
        if (autoSave) return false;

        QMessageBox msg;
        msg.setText(trUtf8("Файл не найден"));
        msg.setWindowTitle( QApplication::instance()->applicationName() );
        msg.setInformativeText(fileName);
        msg.setStandardButtons( QMessageBox::Retry | QMessageBox::Cancel);
        msg.setIcon( QMessageBox::Critical );
        int result = msg.exec();
        if (result == QMessageBox::Retry)
            return openDocument();
        return false;
    }

    if (!file->open(QFile::ReadWrite))
    {
        if (autoSave) return false;

        QMessageBox msg;
        if (file->isBusy() && file->isNotified() && busy)
        {
            *busy = true;
            return false;
        } else if (file->isBusy())
            msg.setText(trUtf8("Файл заблокирован другим процессом"));
        else
            msg.setText(trUtf8("Ошибка открытия файла"));

        msg.setWindowTitle( QApplication::instance()->applicationName() );
        msg.setInformativeText(fileName);
        msg.setStandardButtons( QMessageBox::Retry | QMessageBox::Cancel);
        msg.setIcon( QMessageBox::Critical );
        int result = msg.exec();
        if (result == QMessageBox::Retry)
            return openDocument(fileName);
        return false;
    }

    if (!scene_->document()->loadDocument(file.data()) )
    {
        if (autoSave) return false;

        QMessageBox msg;
        msg.setText(trUtf8("Формат не распознан. Ошибка при чтении файла"));
        msg.setWindowTitle( QApplication::instance()->applicationName() );
        msg.setInformativeText(fileName);
        msg.setStandardButtons( QMessageBox::Retry | QMessageBox::Cancel);
        msg.setIcon( QMessageBox::Critical );
        int result = msg.exec();
        if (result == QMessageBox::Retry)
            return openDocument(fileName);
        return false;
    }

    if (autoSave) return true;

    scene_->document()->clearModify();
    scene_->document()->setFile(fileName, file);


    QString path = QFileInfo(fileName).absolutePath();
    makeNativePath(path);
    QSettings().setValue("default_path", path);
    updateMRU(fileName);

    updateTitle();
    return true;
}

bool MainWindow::openAutoSaveDocument(QString openAs)
{
    if (openDocument(openAs, 0, true))
    {
        scene_->document()->clearModify();
        scene_->document()->setOriginalFile();
        return true;
    }

    return false;
}

void MainWindow::onSaveDocument()
{
    saveDocument();
}

bool MainWindow::saveDocument()
{
    auto autoRestored = scene_->document()->autoRestored();
    auto fname = scene_->document()->fileName();
    if (fname.isEmpty() || fname.toLower().endsWith(".skt") || autoRestored)
    {
        return saveAsDocument();
    }
    else
    {
        return saveCurrentDocument();
    }
}

bool MainWindow::saveAsDocument()
{    
    QString defaultPath = scene_->document()->fileName();
    if (defaultPath.isEmpty())
    {
        QSettings sett;
        defaultPath = "c:\\";
        defaultPath = sett.value("default_path", defaultPath).toString();
    } else if (defaultPath.toLower().endsWith(".skt"))
    {
        defaultPath.truncate( defaultPath.length() - 4);
        defaultPath += ".sktx";
    }

    QString fileName = QFileDialog::getSaveFileName(this,
        trUtf8("Сохранить файл"), // caption
        defaultPath, trUtf8("Natrix files (*.sktx)"));
    makeNativePath(fileName);

    if (fileName.isEmpty()) return false;
    return saveDocument(fileName);
}

void MainWindow::onSaveAsDocument()
{
    saveAsDocument();
}

bool MainWindow::saveDocument(QString fileName)
{
    QSharedPointer<ExclusiveFile> file(new ExclusiveFile{fileName});
    if (!file->open(QFile::ReadWrite))
    {
        if (file->isNotified())
            return false;

        QMessageBox msg;
        if (file->isBusy())
            msg.setText(trUtf8("Файл заблокирован другим процессом"));
        else
            msg.setText(trUtf8("Ошибка открытия файла"));

        msg.setWindowTitle( QApplication::instance()->applicationName() );
        msg.setInformativeText(fileName);
        msg.setStandardButtons( QMessageBox::Retry | QMessageBox::Cancel);
        msg.setIcon( QMessageBox::Critical );
        int result = msg.exec();
        if (result == QMessageBox::Retry)
            return saveAsDocument();
        return false;
    }

    scene_->document()->setFile(fileName, file);
    saveCurrentDocument();

    QString path = QFileInfo(fileName).absolutePath();
    makeNativePath(path);
    QSettings().setValue("default_path", path);

    updateMRU(fileName);
    return true;
}

bool MainWindow::saveCurrentDocument()
{
    QSharedPointer<ExclusiveFile> file = scene_->document()->file();
    if (!scene_->document()->saveDocument(file.data()))
    {
        QMessageBox msg;
        msg.setText(trUtf8("Ошибка программы при записи файла"));
        msg.setWindowTitle( QApplication::instance()->applicationName() );
        msg.setInformativeText(scene_->document()->fileName());
        msg.setStandardButtons( QMessageBox::Retry | QMessageBox::Cancel);
        msg.setIcon( QMessageBox::Critical );
        int result = msg.exec();
        if (result == QMessageBox::Retry)
            return saveAsDocument();
        return false;
    }

    scene_->document()->clearModify();

    updateTitle();
    return true;
}

void MainWindow::updateMRU(QString fileName)
{
    // добавляем новое имя в список
    if (!fileName.isEmpty())
    {
        foreach(auto line, mostRecentlyUsed_)
        {
            if (fileName.toLower() == line.toLower())
            {
                mostRecentlyUsed_.removeOne(line);
                break;
            }
        }

        mostRecentlyUsed_.push_front(fileName);
    }

    while (mostRecentlyUsed_.size()>10)
        mostRecentlyUsed_.takeLast();

    // сохраняем для следующих запусков
    QSettings sett;
    sett.setValue("mru", mostRecentlyUsed_);

    // дальше работаем с локальной копией
    QStringList mru = mostRecentlyUsed_;

    // если добавили файл, то показывать его не стоит
    if (mru.size() && !fileName.isEmpty())
        mru.takeFirst();

    QMenu *fileMenu = menuBar()->actions().at(0)->menu();
    int start = fileMenu->actions().indexOf( ui->action1_MRU );
    Q_ASSERT(start != -1);

    // во время работы программы этот список только
    // растет и никогда не уменьшается
    int count = mru.size();
    for(int index=start, subcount = count; subcount --> 0; ++index)
    {
        if (fileMenu->actions().at(index)->isSeparator())
        {
            QAction* action = new QAction("x. MRU item", this);
            fileMenu->insertAction(fileMenu->actions().at(index), action);
            connect(action, SIGNAL(triggered()), this, SLOT(onOpenMRUDocument()));
        }
    }

    // заполнение меню
    if (count==0)
    {
        QAction* action = fileMenu->actions().at(start);
        action->setText( trUtf8("-- пусто --"));
        action->setEnabled( false );
    }    
    else for(int index=0; index < count; ++index)
    {
        QAction *action = fileMenu->actions().at(start + index);
        action->setEnabled( true );

        auto path = makeNativePath(mru[index]);
        action->setData(path);

        QString title = index < 9 ? QString("&%1. %2")
                                  : QString("%1. %2");
        title = title.arg(1+index).arg(path);
        action->setText(title);
    }
}

void MainWindow::onOpenMRUDocument()
{
    QAction *action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);

    if (!saveIfModified())
        return;

    QString fileName = action->data().toString();

    bool busy = false;
    if (!openDocument(fileName, &busy))
    {
        if (busy)
            this->close();
    }
}

namespace
{
QPrinter* getPrinter()
{
    QSettings sett;
    QString name = sett.value("printer").toString();

    QPrinterInfo def = QPrinterInfo::defaultPrinter();
    foreach(QPrinterInfo inf, QPrinterInfo::availablePrinters())
    {
        if (inf.printerName() == name)
        {
            def = inf;
            break;
        }
    }

    return new QPrinter(def, QPrinter::HighResolution);
}
}

void MainWindow::onPrint()
{
    scene_->manipulator()->cancel();

    // если DEMO -- блокировка
    blockIfNotRegistered(0);

    QScopedPointer<QPrinter> printer(getPrinter());
    QPrintDialog dialog{printer.data(), this};
    if (dialog.exec() != QDialog::Accepted) return;

printAgain:
    // printing
    if (printPage(printer.data()))
    {
        QSettings sett;
        sett.setValue("printer", printer->printerName());
        return;
    }

    QMessageBox box;
    box.setText(QObject::trUtf8("Печать завершилась неудачей"));
    box.setInformativeText(QObject::trUtf8("Хотите повторить попытку?"));
    box.setStandardButtons(QMessageBox::Retry|QMessageBox::Cancel);
    if (box.exec() == QMessageBox::Retry)
        goto printAgain;
}

void MainWindow::onPrintPreview()
{
    scene_->manipulator()->cancel();

    // если DEMO -- блокировка
    blockIfNotRegistered(0);

    QScopedPointer<QPrinter> printer(getPrinter());
    QPrintPreviewDialog dialog{printer.data(), this};
    connect(&dialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printPage(QPrinter*)));
    if (dialog.exec() != QDialog::Accepted) return;
}

void MainWindow::onAbout()
{
    AboutDialog dialog;

    connect(&dialog, SIGNAL(askToBuy()), SLOT(onBuy()));
    connect(&dialog, SIGNAL(askToEnterSerial()), SLOT(onEnterSerial()));
    connect(this, SIGNAL(nowRegistered()), &dialog, SLOT(nowRegistered()));

    dialog.exec();
}

void MainWindow::onAboutQt()
{
    QApplication::aboutQt();
}

void MainWindow::onTogglePropertyTab()
{    
    QSettings().setValue("PropertyWindowVisible", (int)ui->zoneC_2->isHidden());
    if (!ui->zoneC_2->isHidden())
        ui->zoneC_2->hide();
    else
        ui->zoneC_2->show();
}

void MainWindow::onToggleToolsTab()
{
    QSettings().setValue("ToolsWindowVisible", (int)ui->zoneA_2->isHidden());
    if (!ui->zoneA_2->isHidden())
        ui->zoneA_2->hide();
    else
        ui->zoneA_2->show();
}

void MainWindow::onUnregister()
{
    Security::storeRegistrationInfo("", "", "");
    QMenu* helpMenu =  menuBar()->actions().last()->menu();
    helpMenu->insertAction(ui->actionUnregister,
                           ui->actionRegister);
    helpMenu->removeAction(ui->actionUnregister);
    registered = false;
    updateTitle();
}

void MainWindow::checkRegistered()
{
    int nowRegistered = Security::checkCode(
                        Security::retrieveRegistrationInfo().serial );

    if (nowRegistered)
    {
        this->nowRegistered();
        registered = nowRegistered;

        if (!registered)
        {
            QMenu* helpMenu = menuBar()->actions().last()->menu();
#ifndef NDEBUG
            helpMenu->insertAction(ui->actionRegister,
                               ui->actionUnregister);
#endif
            helpMenu->removeAction(ui->actionRegister);
        }
    }

    updateTitle();
}

void MainWindow::onEnterSerial()
{
    RegisterDialog dialog;
    if (dialog.exec()==1)
    {
        checkRegistered();
    }
}

void MainWindow::onBuy()
{
    QString url = QCoreApplication::instance()->organizationDomain();
    url = QString("http://%1/purchase.html").arg(url);
    QDesktopServices::openUrl(url);
}

bool MainWindow::printPage(QPrinter* printer)
{
    printer->setDocName( QString("Natrix %1").arg(scene_->document()->fileName()) );
    printer->setOrientation(QPrinter::Landscape);
    printer->setFullPage(true);
    printer->setMargins({0,0,0,0});

    // рисование
    QPainter painter;
    bool ready = painter.begin(printer);
    if (!ready) return false;

    scene_->render(&painter, printer);

    return true;
}

void MainWindow::onExport()
{
    scene_->manipulator()->cancel();

    // если DEMO -- блокировка
    blockIfNotRegistered(0);

    ExportDialog dialog(scene_, this);
    dialog.exec();
}

void MainWindow::updateUIEvent()
{
    ui->actionSave->setEnabled(
                scene_->document()->modified()
                || scene_->document()->isNewDocument());

    // выделен ли хоть один элемент
    bool hasSelection = !scene_->selectedObjects().empty();
    ui->actionDelete->setEnabled( hasSelection );
    ui->actionSelectNone->setEnabled( hasSelection );

    bool someUnselected = scene_->hasUnselected();
    ui->actionSelectAll->setEnabled( someUnselected );

    updateTitle();
}

MainWindow::~MainWindow()
{
    scene_->manipulator()->saveLogs();
    delete scene_;
    delete ui;
}

void MainWindow::toggle640x480()
{
    setGeometry(192, 124, 640, 500);
}

void MainWindow::toggle800x600()
{
    setGeometry(192, 124, 800, 620);
}

