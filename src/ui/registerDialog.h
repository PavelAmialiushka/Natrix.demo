#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
    class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = 0);
    ~RegisterDialog();

public slots:
    void onSerialChanged(QString);
    void onAccept();
    void onTimer();

private:
    QTimer             *timer;
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
