#include "SPSDialog.hpp"
#include "ui_SPSDialog.h"

#include <QFileDialog>

SPSDialog::SPSDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SPSDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(ui->buttonBox->Ok)->setEnabled(false);
    QObject::connect(ui->r_sps, &QPushButton::clicked, this, &SPSDialog::chooseRSPS);
    QObject::connect(ui->s_sps, &QPushButton::clicked, this, &SPSDialog::chooseSSPS);
    QObject::connect(ui->x_sps, &QPushButton::clicked, this, &SPSDialog::chooseXSPS);
    QObject::connect(ui->rev_21, &QRadioButton::clicked, this, &SPSDialog::setRev21);
    QObject::connect(ui->rev_0, &QRadioButton::clicked, this, &SPSDialog::setRev0);
}

SPSDialog::~SPSDialog()
{
    delete ui;
}

void SPSDialog::chooseRSPS()
{
    r_sps_name = QFileDialog::getOpenFileName(this, tr("Choose R SPS file"));
    check_names();
}

void SPSDialog::chooseSSPS()
{
    s_sps_name = QFileDialog::getOpenFileName(this, tr("Choose S SPS file"));
    check_names();
}

void SPSDialog::chooseXSPS()
{
    x_sps_name = QFileDialog::getOpenFileName(this, tr("Choose X SPS file"));
    check_names();
}

void SPSDialog::check_names()
{
    if (!r_sps_name.isEmpty() && !s_sps_name.isEmpty() && !x_sps_name.isEmpty())
        ui->buttonBox->button(ui->buttonBox->Ok)->setEnabled(true);
}

void SPSDialog::setRev0()
{
    sps_rev_2 = false;
}

void SPSDialog::setRev21()
{
    sps_rev_2 = true;
}
