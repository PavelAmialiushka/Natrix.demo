#include "aboutDialog.h"
#include "ui_aboutDialog.h"

#include "secman/securityStorage.h"

#include <qdebug>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->labelAppName->setText(
                ui->labelAppName->text().arg(QApplication::instance()->applicationVersion()));

    connect(ui->pushBuy, SIGNAL(clicked()), SIGNAL(askToBuy()));
    connect(ui->pushEnterSerial, SIGNAL(clicked()), SIGNAL(askToEnterSerial()));
    connect(ui->pushClose, SIGNAL(clicked()), SLOT(accept()));

    ui->labelURL->setText( QString("<a href=\"http://%1\">%1</a>").arg(
                               QApplication::instance()->organizationDomain()) );

    nowRegistered();
}

void AboutDialog::nowRegistered()
{
    auto info = Security::retrieveRegistrationInfo();
    if (!info.serial.isEmpty())
    {
        ui->labelName->setText( info.name );
        ui->labelCompany->setText( info.organization );
        ui->labelSerial->setText( info.serial );

        ui->pushBuy->hide();
        ui->pushEnterSerial->hide();
    }}

AboutDialog::~AboutDialog()
{
    delete ui;
}
