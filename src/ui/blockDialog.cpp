#include "blockDialog.h"
#include "ui_blockDialog.h"

#include <QDir>
#include <QTimer>
#include <QDateTime>
#include <QCloseEvent>

#include <qDebug>
#include <QSettings>

#include "registerDialog.h"

BlockDialog::BlockDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::blockDialog)
{
    ui->setupUi(this);

    titleTimer = new QTimer(this);
    timeoutTimer.start();

    // disable close button
    setWindowFlags( windowFlags() & ~Qt::WindowCloseButtonHint );

    connect(ui->pushClose, SIGNAL(clicked()), SLOT(close()));
    connect(titleTimer, SIGNAL(timeout()), SLOT(onUpdateButtonTitle()));

    connect(ui->pushBuy, SIGNAL(clicked()), SIGNAL(askToBuy()));
    connect(ui->pushEnterSerial, SIGNAL(clicked()), SIGNAL(askToEnterSerial()));

    ui->labelText1->setText(
                ui->labelText1->text().arg( daysBeforeDemoEnds() ));

    titleTimer->setSingleShot( false );
    titleTimer->start(1000);

    if (daysBeforeDemoEnds() > 0)
        ui->labelText2->hide();
    else
        ui->labelText1->hide();

    auto dai = daysAfterInstall();
    timeout = dai < 2  ? 15 :
              dai < 10  ? 30 :
              dai < 30 ? 60
                       : 120;
}

BlockDialog::~BlockDialog()
{
    delete ui;
}

void BlockDialog::closeEvent(QCloseEvent * evt)
{
    if (secondsRest() > 0)
        evt->ignore();
}

void BlockDialog::keyPressEvent(QKeyEvent * evt)
{
    if (evt->key()==Qt::Key_Escape)
        evt->ignore();
}

void BlockDialog::nowRegistered()
{
    timeout = 0;
    done(1);
}

int BlockDialog::secondsRest()
{
    return timeout-timeoutTimer.elapsed()/1000;
}

void BlockDialog::onUpdateButtonTitle()
{
    int rest = secondsRest();
    if (rest> 0)
    {
        ui->pushClose->setText( trUtf8("%1").arg(rest) );
    } else
    {
        ui->pushClose->setText( trUtf8("Закрыть") );
        ui->pushClose->setEnabled( 1 );
    }
}

static QDateTime currentDateTime()
{
    QDateTime mostRecent;

    QFileInfoList list = QDir::temp().entryInfoList();
    if (list.isEmpty())
        mostRecent = QDateTime::currentDateTime();
    else
        mostRecent = list.first().created();

    foreach(QFileInfo info, list)
    {
        if (info.isFile())
        {
            mostRecent = qMax(mostRecent,
                              info.lastModified());
        }
    }
    return mostRecent;
}

static QDateTime installDateTime()
{
    static int firstRun = 1;
    static QDateTime time;
    if (firstRun)
    {
        QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
                      QSettings::NativeFormat);

        QString appData = reg.value("AppData").toString();
        QString appName = QApplication::instance()->applicationName();

        QFileInfo appDir(appData + "\\"
#ifndef NDEBUG
                         "TEMP_"
#endif
                         + appName);

        if (!appDir.exists())
        {
            QDir().mkpath(appDir.filePath());
        }
        time = appDir.created();
        firstRun = 0;
    }

    return time;
}

int BlockDialog::shallShowDialog(bool onStart)
{
    if (onStart)
        return 1;
    else
        return daysBeforeDemoEnds() < 0;
}

int BlockDialog::daysBeforeDemoEnds()
{
    return 30 - daysAfterInstall();
}

int BlockDialog::daysAfterInstall()
{
    QDateTime current = currentDateTime();
    QDateTime install = installDateTime();

#ifdef NDEBUG
    return install.daysTo(current);
#else
    return 1;
#endif
}

