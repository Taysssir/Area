#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , robot_(nullptr)
{
    ui->setupUi(this);
    connect(this, SIGNAL(maj(QString)), this, SLOT(majHASH(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_run_clicked()
{
     if(robot_)
         robot_->launch(ui->lineEdit_process->text().toStdString());
}

void MainWindow::on_pushButton_stop_clicked()
{
     if(robot_)
         robot_->requestTermination(ui->lineEdit_process->text().toStdString());
}

void MainWindow::on_pushButton_kill_clicked()
{
     if(robot_)
         robot_->pkill(ui->lineEdit_process->text().toStdString());
}

void MainWindow::on_pushButton_pids_clicked()
{
     if(robot_)
         robot_->pidof(ui->lineEdit_process->text().toStdString());
}

void MainWindow::on_pushButton_connect_clicked()
{
    if(robot_)
        delete robot_;
    robot_ = new skynet::Robot(ui->lineEdit_ip->text().toStdString(), 1234, this);
}

void MainWindow::onConnected()
{
    ui->label_status->setText("Connected !");
}

void MainWindow::onDisconnected()
{
    ui->label_status->setText("Disconnected !");
}

void MainWindow::onReceive(skynet::PidofResponse const& event)
{
    QString pids;
    for(int i=0;i<event.pid_size();i++)
    {
        pids+= QString::number(event.pid(i)) + " ";
    }
    ui->lineEdit_pids->setText(pids);
}

void MainWindow::onReceive(skynet::ConfigHashResponse const& event)
{
    QString hash;
    for(int i=0;i<event.files_size();i++)
    {
        hash += " path : " + QString(event.files(i).path().c_str()) + " ";
        hash += " hash : " + QString(event.files(i).hash().c_str()) + "\n";
    }
    emit maj(hash);
}


void MainWindow::on_pushButton_hash_clicked()
{
   if(robot_)
       robot_->getConfigHash();
}

void MainWindow::majHASH(QString const& str)
{
    ui->textEdit_hash->setPlainText(str);
}
