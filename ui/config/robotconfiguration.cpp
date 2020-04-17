#include <QMessageBox>

#include "robotconfiguration.h"
#include "ui_robotconfiguration.h"
#include "sentinel-admin_core/events.h"

RobotConfiguration::RobotConfiguration(core::AppInstance &instance, QWidget *parent)
    : QWidget(parent)
    , core::EventClient<RobotConfiguration>(instance)
    , ui(new Ui::RobotConfiguration)
{
    ui->setupUi(this);
    //ui->checkBox_dispeldoubt->setChecked(instance_.configurationSettings()->activeBackToPatrol());
    //ui->timeEdit_dispeldoubt->setTime(instance_.configurationSettings()->timeBackToPatrol());

    //ui->checkBox_axiscamera->setChecked(instance_.configurationSettings()->axisCamera());
    //ui->checkBox_thermiccamera->setChecked(instance_.configurationSettings()->thermicCamera());
    //ui->checkBox_backupcamera->setChecked(instance_.configurationSettings()->backupCamera());
    //ui->pushButton_apply->setEnabled(false);
}

RobotConfiguration::~RobotConfiguration()
{
    delete ui;
}

void RobotConfiguration::on_pushButton_apply_clicked()
{
    //instance_.configurationSettings()->setOptionBackToPatrol(
    //    ui->checkBox_dispeldoubt->isChecked(),
    //    ui->timeEdit_dispeldoubt->time());

    //instance_.configurationSettings()->setAxisCamera(ui->checkBox_axiscamera->isChecked());
    //instance_.configurationSettings()->setThermicCamera(ui->checkBox_thermiccamera->isChecked());
    //instance_.configurationSettings()->setBackupCamera(ui->checkBox_backupcamera->isChecked());

    //ui->pushButton_apply->setEnabled(false);
}

void RobotConfiguration::on_checkBox_dispeldoubt_clicked(bool)
{
    ui->pushButton_apply->setEnabled(true);
}

void RobotConfiguration::on_timeEdit_dispeldoubt_timeChanged(const QTime &)
{
    ui->pushButton_apply->setEnabled(true);
}

void RobotConfiguration::on_checkBox_axiscamera_clicked(bool)
{
    ui->pushButton_apply->setEnabled(true);
}

void RobotConfiguration::on_checkBox_thermiccamera_clicked(bool)
{
    ui->pushButton_apply->setEnabled(true);
}

void RobotConfiguration::on_checkBox_backupcamera_clicked(bool)
{
    ui->pushButton_apply->setEnabled(true);
}
