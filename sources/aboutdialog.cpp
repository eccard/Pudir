/***********************************************************************************************
 * Pudir  - A pusher of directory to Android
 *   Version: 1.0
 * Copyright 2015 eccard
 * MIT License
 ************************************************************************************************/
#include "headers/aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButton_clicked()
{
    close();
}
