#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lh_key.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_editDate = findChild<QLineEdit*>("ctl_le_date");
    m_editCompany = findChild<QLineEdit*>("ctl_le_company");
    m_editKey= findChild<QLineEdit*>("ctl_le_key");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    LHKeyGenerator genor(m_editDate->text().toStdString(), m_editCompany->text().toStdString());
    if(genor.Check())
    {
        m_editKey->setText(genor.Generate().c_str());
    }
    else
    {
        QMessageBox::information(this, "Error", QString::fromLocal8Bit("输入的参数有误"), QMessageBox::Yes);
    }
}
