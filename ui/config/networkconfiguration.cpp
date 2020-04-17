#include <QMessageBox>

#include "networkconfiguration.h"
#include "ui_networkconfiguration.h"
#include "core/helper.h"
#include "sentinel-admin_core/events.h"
#include "sentinel-admin_core/qt_utils.h"
#include "sentinel-admin_core/constants.h"


#include <QDebug>

namespace network_configuration
{
namespace tag
{
constexpr char ANTENNA_KEYS[] = "antenna_ip";
}
}

NetworkConfiguration::NetworkConfiguration(core::AppInstance & instance, QWidget *parent)
    : QWidget(parent)
    , core::EventClient<NetworkConfiguration>(instance)
    , ui(new Ui::NetworkConfiguration)
    //, mPingsService(new PingsService(instance))
{
    ui->setupUi(this);
    //ui->lineEdit_ip->setText( instance_.configurationSettings()->robotIP() );
    //ui->lineEdit_antenna->setText( instance_.configurationSettings()->antennas().join(", ") );
    //ui->lineEdit_antenna->setToolTip(tr("enter_list_of_antennas_ip_separte_with_comma"));
    //ui->lineEdit_ip_thermiccamera->setText( instance.configurationSettings()->thermicCameraPort() );
    //ui->lineEdit_ip_yocto->setText( instance.configurationSettings()->yoctoPort() );
    //ui->pushButton_apply->setEnabled(false);
    //// JBE HDA
    /// remove this button for the moment
    //ui->pushButton_networkCheck->setVisible(false);
    model_antenna_ip = new QStringListModel();
    ui->label_ip_thermal_camera->setVisible(false);
    ui->lineEdit_ip_thermiccamera->setVisible(false);
    ui->label_ip_yocto->setVisible(false);
    ui->lineEdit_ip_yocto->setVisible(false);

    setupAntennaIpList();
    //ui->splitter_2->hide();
}

NetworkConfiguration::~NetworkConfiguration()
{
    delete ui;
}

void NetworkConfiguration::on_pushButton_apply_clicked()
{

    // reset output
    //ui->label_error->setText("");
    //QString robotIP = ui->lineEdit_ip->text();
    //if(!robotIP.size())
    //{
    //    ui->label_error->setText(tr("configuration_error__enter_valid_robot_ip"));
    //    return ;
    //}
    //if(!core::isValidIP(robotIP))
    //{
    //    ui->label_error->setText(tr("configuration_error__robot_ip_not_valid"));
    //    return ;
    //}
    //QString thermicCameraPort = ui->lineEdit_ip_thermiccamera->text();
    //QRegExp re("\\d*");
    //if(!re.exactMatch(thermicCameraPort))
    //{
    //    ui->label_error->setText(tr("configuration_error__thermic_camera_port_is_not_valid"));
    //    return ;
    //}
    //QString yoctoport = ui->lineEdit_ip_yocto->text();
    //if(!re.exactMatch(yoctoport))
    //{
    //    ui->label_error->setText(tr("configuration_error__yocto_port_is_not_valid"));
    //    return ;
    //}
    //QString antennaIP = ui->lineEdit_antenna->text();
    //antennaIP = antennaIP.replace(" ", ";");
    //antennaIP = antennaIP.replace(",", ";");
    //antennaIP = antennaIP.replace(";;", ";");
    //QStringList ips = antennaIP.split(";");
    //int index = 0;
    //for(auto const& ip : ips)
    //{
    //    if(ip.size())
    //    {
    //        if(!core::isValidIP(ip))
    //        {
    //            ui->label_error->setText(tr("configuration_error__ip_antenna_%1_is_not_valid")
    //                        .arg(index+1));
    //            return ;
    //        }
    //        index++;
    //    }
    //}
    QStringList antenna_ip = model_antenna_ip->stringList();

    //QString lastRobotIP = instance_.configurationSettings()->robotIP();
    bool found_settings = false;
    for(int i(0); i < instance_.configuration().settings()->settings().size() ;++i)
    {
        if(instance_.configuration().settings()->settings(i).keys() == network_configuration::tag::ANTENNA_KEYS)
        {
            instance_.configuration().settings()->mutable_settings(i)->set_value(antenna_ip.join(',').toStdString());
            found_settings = true;
        }
    }
    if(!found_settings)
    {
        ::eos::JSONSettings *JSON = instance_.configuration().settings()->add_settings();
        JSON->set_keys(network_configuration::tag::ANTENNA_KEYS);
        JSON->set_value(antenna_ip.join(',').toStdString());
    }
    instance_.configuration().save(eos::Configuration::Settings);
    //QString lastRobotIP = instance_.configurationSettings()->robotIP();
    //instance_.configurationSettings()->setRobotIP( robotIP );
    //instance_.configurationSettings()->setAntennas( ips );
    ////instance_.configurationSettings()->setThermicCameraIP( thermicCameraIP );
    //instance_.configurationSettings()->setThermicCameraPort(thermicCameraPort );
    //instance_.configurationSettings()->setYoctoPort( yoctoport );
    //if(lastRobotIP != robotIP)
    //    emitEvent<core::IPChanged>();
    //ui->pushButton_apply->setEnabled(false);
}

void NetworkConfiguration::on_lineEdit_antenna_textChanged(const QString &)
{
    ui->pushButton_apply->setEnabled(true);
}

void NetworkConfiguration::on_lineEdit_ip_thermiccamera_textChanged(const QString &)
{
    ui->pushButton_apply->setEnabled(true);
}

void NetworkConfiguration::on_lineEdit_ip_yocto_textChanged(const QString &)
{
    ui->pushButton_apply->setEnabled(true);
}

void NetworkConfiguration::on_pushButton_clicked()
{
    instance_.saveMemento(instance_.resetMementoIp(constants::APP_NAME),constants::APP_NAME);
}

void NetworkConfiguration::on_pushButton_add_antenna_ip_clicked()
{
    if(!ui->lineEdit_antenna->text().isEmpty()
            && core::isValidIP(ui->lineEdit_antenna->text())
            && !model_antenna_ip->stringList().contains(ui->lineEdit_antenna->text()))
    {
        QStringList list = model_antenna_ip->stringList();
        list.append(ui->lineEdit_antenna->text());
        model_antenna_ip->setStringList(list);
    }
    ui->listView_antenna_ip->setModel(model_antenna_ip);
}

void NetworkConfiguration::on_pushButton_remove_antenna_Ip_clicked()
{
    model_antenna_ip->removeRow(ui->listView_antenna_ip->currentIndex().row());
    ui->listView_antenna_ip->setModel(model_antenna_ip);
}

void NetworkConfiguration::setupAntennaIpList()
{
    eos::SettingsSharedPointer settings = instance_.read_only().configuration().settings();
    for(auto setting : settings->settings())
    {
        if(setting.keys() == network_configuration::tag::ANTENNA_KEYS)
        {
           model_antenna_ip->setStringList(QString::fromStdString(setting.value()).split(","));
        }
    }
    ui->listView_antenna_ip->setModel(model_antenna_ip);
}
