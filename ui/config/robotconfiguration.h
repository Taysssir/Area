#ifndef ROBOTCONFIGURATION_H
#define ROBOTCONFIGURATION_H

#include <QWidget>
#include "core/appinstance.h"

class ConfigurationSettings;

namespace Ui {
class RobotConfiguration;
}

class RobotConfiguration
        : public QWidget
        , public core::EventClient<RobotConfiguration>
{
    Q_OBJECT
    
public:
    explicit RobotConfiguration(core::AppInstance & instance, QWidget *parent = 0);
    ~RobotConfiguration();
    
private slots:
    void on_pushButton_apply_clicked();
    
    void on_checkBox_dispeldoubt_clicked(bool checked);

    void on_timeEdit_dispeldoubt_timeChanged(const QTime &time);

    void on_checkBox_axiscamera_clicked(bool checked);

    void on_checkBox_thermiccamera_clicked(bool checked);

    void on_checkBox_backupcamera_clicked(bool checked);

private:
    Ui::RobotConfiguration *ui;
};

#endif // ROBOTCONFIGURATION_H
