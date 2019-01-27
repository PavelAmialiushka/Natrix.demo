#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

signals:
    void askToEnterSerial();
    void askToBuy();

public slots:
    void nowRegistered();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
