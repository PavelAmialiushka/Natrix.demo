#ifndef BLOCKDIALOG_H
#define BLOCKDIALOG_H

#include <QDialog>
#include <QElapsedTimer>

namespace Ui {
    class blockDialog;
}

class BlockDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BlockDialog(QWidget *parent = 0);
    ~BlockDialog();

public:
    static int daysBeforeDemoEnds();
    static int daysAfterInstall();
    static int shallShowDialog(bool onStart);
    int secondsRest();

signals:
    void askToEnterSerial();
    void askToBuy();

public slots:
    void closeEvent(QCloseEvent *);
    void keyPressEvent(QKeyEvent *);
    void onUpdateButtonTitle();
    void nowRegistered();

private:
    Ui::blockDialog *ui;
    QTimer          *titleTimer;
    int             timeout;
    QElapsedTimer   timeoutTimer;
};

#endif // BLOCKDIALOG_H
