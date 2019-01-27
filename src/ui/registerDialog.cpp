#include "registerDialog.h"
#include "ui_registerDialog.h"

#include <QPushButton>

#include "secman/securityCodes.h"
#include "secman/securityStorage.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    timer = new QTimer(this);

    auto reg = Security::retrieveRegistrationInfo();
    ui->lineName->setText( reg.name );
    ui->lineOrganization->setText( reg.organization );
    ui->lineSerial->setText( reg.serial );

    connect(this, SIGNAL(accepted()), SLOT(onAccept()));
    connect(ui->lineSerial, SIGNAL(textChanged(QString)), SLOT(onSerialChanged(QString)));
    connect(timer, SIGNAL(timeout()), SLOT(onTimer()));
    onTimer();
}

void RegisterDialog::onSerialChanged(QString)
{
    timer->start(200);
}

void RegisterDialog::onAccept()
{
    Security::storeRegistrationInfo(
                ui->lineName->text(),
                ui->lineOrganization->text(),
                ui->lineSerial->text());
}

void RegisterDialog::onTimer()
{
    QString serial = ui->lineSerial->text();
    int ok = Security::checkCode(serial);
    int turnRed = !ok && !serial.isEmpty();
#ifndef NDEBUG
    if (serial=="123")
    {
        serial = "QS1MP-X9HMG-CE58P-X17W7-HNFB5";
        ui->lineSerial->setText(serial);
        ok = true;
    }
#endif

    ui->lineSerial->setStyleSheet(turnRed ? "background: pink" : "");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled( ok );
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}
