#include "frmmain.h"
#include "ui_frmmain.h"

frmMain::frmMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(3);
}

frmMain::~frmMain()
{
    delete ui;
}
