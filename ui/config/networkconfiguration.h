#ifndef NETWORKCONFIGURATION_H
#define NETWORKCONFIGURATION_H

#include <QWidget>
#include "core/appinstance.h"
#include "QStringListModel"

namespace Ui {
    class NetworkConfiguration;
}

class NetworkConfiguration
        : public QWidget
        , public core::EventClient<NetworkConfiguration>
{
    Q_OBJECT

public:
    explicit NetworkConfiguration(core::AppInstance & instance, QWidget *parent = 0);
    ~NetworkConfiguration();

private slots:
    void on_pushButton_apply_clicked();
    void on_lineEdit_antenna_textChanged(const QString &arg1);
    void on_lineEdit_ip_thermiccamera_textChanged(const QString &arg1);
    void on_lineEdit_ip_yocto_textChanged(const QString &arg1);
    void on_pushButton_clicked();
    void on_pushButton_add_antenna_ip_clicked();
    void on_pushButton_remove_antenna_Ip_clicked();

private:
    Ui::NetworkConfiguration *ui;
    QStringListModel* model_antenna_ip ;

    void setupAntennaIpList();
};

#endif // NETWORKCONFIGURATION_H
