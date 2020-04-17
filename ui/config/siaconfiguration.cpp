#include "siaconfiguration.h"
#include "ui_siaconfiguration.h"

#include <QDebug>
#include <QJsonDocument>

namespace sia
{
namespace tag
{
constexpr char KEYS[] = "siadc09";
constexpr char TRANSMITTER[] = "transmitter";
constexpr char ACCT[] = "acct_number";
constexpr char LPREF[] = "Lpref_numer";
constexpr char RRCVR[] = "Rrcvr_number";
constexpr char SERVER[] = "server";
constexpr char PRIMARY_SERVER[] = "primary server";
constexpr char PRIMARY_SERVER_IP[] = "ip_principal_server";
constexpr char PRIMARY_SERVER_PORT[] = "port_principal_server";
constexpr char SECONDARY_SERVER[] = "secondary_server";
constexpr char SECONDARY_SERVER_IP[] = "ip_secondary_server";
constexpr char SECONDARY_SERVER_PORT[] = "port_secondary_server";
constexpr char MESSAGE[] = "message";
constexpr char DICTIONNARY[] = "is_fulldictionnary";
constexpr char TIMESTAMP[] = "need_timestamp";
constexpr char UPPERCASE[] = "need_uppercase";
constexpr char ENCRYPTION[] = "need_encryption";
constexpr char POLLING[] = "time_between_polling";
constexpr char NUMBER_NAK[] ="number_of_nak";
constexpr char NUMBER_NOACK[] = "number_of_missing_answer";
constexpr char TIMEOUT[] = "time_between_timeout";
constexpr char NUMBER_DUH[] = "number_of_duh";
constexpr char ALARM_SPECIFICATION[] = "alarm_specification";
constexpr char AREA_PRECISION[] = "area_precision";
constexpr char FEEDBACK_TIMEOUT[] = "feedback_timeout";
constexpr char ALARM_UPDATE_INTERVAL[] = "alarm_update_interval";
}
namespace alarm_specs_defaults
{
const unsigned int ALARM_AREA_PRECISION_DEFAULT = 2; // 2 meters
const unsigned int ALARM_FEEDBACK_TIMEOUT_DEFAULT = 0; // 0 seconds
const unsigned int ALARM_UPDATE_INTERVAL_DEFAULT = 5; // 5 seconds
}
}

SiaConfiguration::SiaConfiguration(core::AppInstance & instance, QWidget *parent) :
    QWidget(parent),
    core::EventClient<SiaConfiguration>(instance),
    ui(new Ui::SiaConfiguration)
{
    ui->setupUi(this);

    // Load default values for alarm specs
    ui->spinBox_alarm_area_precision->setValue(sia::alarm_specs_defaults::ALARM_AREA_PRECISION_DEFAULT);
    ui->spinBox_alarm_feedback_timeout->setValue(sia::alarm_specs_defaults::ALARM_FEEDBACK_TIMEOUT_DEFAULT);
    ui->spinBox_alarm_update_interval->setValue(sia::alarm_specs_defaults::ALARM_UPDATE_INTERVAL_DEFAULT);

    setupSiaData();
}

SiaConfiguration::~SiaConfiguration()
{
    delete ui;
}

void SiaConfiguration::setupSiaData()
{
    eos::SettingsSharedPointer settings = instance_.read_only().configuration().settings();
    QJsonDocument doc ;
    QJsonObject global_settings;
    for(auto setting : settings->settings())
    {
        if(setting.keys() == sia::tag::KEYS)
        {
            doc =  QJsonDocument::fromJson(QString::fromStdString(setting.value()).toUtf8());
        }
    }
    if (doc.isObject())
    {
        global_settings = doc.object();
        loadTransmitters(global_settings[sia::tag::TRANSMITTER].toObject());
        loadServer(global_settings[sia::tag::SERVER].toObject()[sia::tag::PRIMARY_SERVER].toObject(),PRIMARY_SERVER);
        loadServer(global_settings[sia::tag::SERVER].toObject()[sia::tag::SECONDARY_SERVER].toObject(),SECONDARY_SERVER);
        loadMessage(global_settings[sia::tag::MESSAGE].toObject());
        loadAlarmSpecification(global_settings[sia::tag::ALARM_SPECIFICATION].toObject());
    }
}

void SiaConfiguration::on_pushButtonSave_clicked()
{
    QJsonObject global_settings, transmitter_settings, server_settings, primary_server, secondary_server, message_settings, alarm_specs;
    saveTransmitterInformation(transmitter_settings);
    global_settings[sia::tag::TRANSMITTER] = transmitter_settings;
    saveServerInformation(primary_server,PRIMARY_SERVER);
    saveServerInformation(secondary_server,SECONDARY_SERVER);
    server_settings[sia::tag::PRIMARY_SERVER] = primary_server;
    server_settings[sia::tag::SECONDARY_SERVER] = secondary_server;
    global_settings[sia::tag::SERVER] = server_settings;
    saveMessageParameters(message_settings);
    saveAlarmSpecification(alarm_specs);
    global_settings[sia::tag::MESSAGE]=message_settings;
    global_settings[sia::tag::ALARM_SPECIFICATION]=alarm_specs;
    // qDebug() << global_settings;
    QJsonDocument doc(global_settings);
    QString string_settings = doc.toJson(QJsonDocument::Compact);

    bool found_settings = false;
    for(int i(0); i < instance_.configuration().settings()->settings().size() ;++i)
    {
        if(instance_.configuration().settings()->settings(i).keys() == sia::tag::KEYS)
        {
            instance_.configuration().settings()->mutable_settings(i)->set_value(string_settings.toStdString());
            found_settings = true;
            // qDebug()<< QString::fromStdString(jsonSetting.value());
        }
    }
    if(!found_settings)
    {
        ::eos::JSONSettings *JSON = instance_.configuration().settings()->add_settings();
        JSON->set_keys(sia::tag::KEYS);
        JSON->set_value(string_settings.toStdString());
    }
    // qDebug()<<string_settings;
    instance_.configuration().save(eos::Configuration::Settings);

}

void SiaConfiguration::saveTransmitterInformation(QJsonObject &json)
{
    json[sia::tag::ACCT] = ui->lineEdit_acct->text();
    json[sia::tag::LPREF] = ui->lineEdit_Lpref->text();
    json[sia::tag::RRCVR] = ui->lineEdit_Rrcvr->text();
}

void SiaConfiguration::saveServerInformation(QJsonObject &json, Type_server type)
{
    switch(type)
    {
    case PRIMARY_SERVER :
        json[sia::tag::PRIMARY_SERVER_IP] = ui->lineEdit_primary_server_ip->text();
        json[sia::tag::PRIMARY_SERVER_PORT] = ui->spinBox_primary_server_port->value();
        break;
    case SECONDARY_SERVER :
        json[sia::tag::SECONDARY_SERVER_IP] = ui->lineEdit_secondary_server_ip->text();
        json[sia::tag::SECONDARY_SERVER_PORT] = ui->spinBox_secondary_server_port->value();
        break;
    }
}

void SiaConfiguration::saveMessageParameters(QJsonObject &json)
{
    json[sia::tag::DICTIONNARY] = ui->radioButton_EOSFull->isChecked();
    json[sia::tag::TIMESTAMP] = ui->radioButton_nak_timestamp_yes->isChecked();
    json[sia::tag::UPPERCASE] = ui->radioButton_casesensitive_uppercase->isChecked() | ui->radioButton_casesensitive_checksum_uppercase->isChecked();
    json[sia::tag::ENCRYPTION] = 0;
    json[sia::tag::POLLING]= ui->timeEdit_polling_time->time().msecsSinceStartOfDay()/1000;
    json[sia::tag::NUMBER_NAK] = ui->spinBox_number_try_nak->value();
    json[sia::tag::NUMBER_NOACK] = ui->spinBox_number_try_no_ack->value();
    json[sia::tag::TIMEOUT] = ui->timeEdit_trytime_between_no_ack->time().msecsSinceStartOfDay()/1000;
    json[sia::tag::NUMBER_DUH] = ui->radioButton_DUH_answer_yes->isChecked();
}

void SiaConfiguration::saveAlarmSpecification(QJsonObject &json)
{
    json[sia::tag::AREA_PRECISION] = ui->spinBox_alarm_area_precision->value();
    json[sia::tag::FEEDBACK_TIMEOUT] = ui->spinBox_alarm_feedback_timeout->value();
    json[sia::tag::ALARM_UPDATE_INTERVAL] = ui->spinBox_alarm_update_interval->value();
}



void SiaConfiguration::on_pushButtonCancel_clicked()
{
    setupSiaData();
}

void SiaConfiguration::loadTransmitters(QJsonObject json)
{
    ui->lineEdit_acct->setText(json.value(sia::tag::ACCT).toString());
    ui->lineEdit_Lpref->setText(json.value(sia::tag::LPREF).toString());
    ui->lineEdit_Rrcvr->setText(json.value(sia::tag::RRCVR).toString());
}

void SiaConfiguration::loadServer(QJsonObject json, Type_server type)
{
    switch(type)
    {
    case PRIMARY_SERVER :
        ui->lineEdit_primary_server_ip->setText(json.value(sia::tag::PRIMARY_SERVER_IP).toString());
        ui->spinBox_primary_server_port->setValue(json.value(sia::tag::PRIMARY_SERVER_PORT).toInt());
        break;
    case SECONDARY_SERVER :
        ui->lineEdit_secondary_server_ip->setText(json.value(sia::tag::SECONDARY_SERVER_IP).toString());
        ui->spinBox_secondary_server_port->setValue(json.value(sia::tag::SECONDARY_SERVER_PORT).toInt());
        break;
    }
}

void SiaConfiguration::loadMessage(QJsonObject json)
{
    if(json.value(sia::tag::DICTIONNARY).toBool())
        ui->radioButton_EOSFull->setChecked(true);
    else
        ui->radioButton_EOSLight->setChecked(true);
    if(json.value(sia::tag::TIMESTAMP).toBool())
        ui->radioButton_nak_timestamp_yes->setChecked(true);
    else
        ui->radioButton_nak_timestamp_no->setChecked(true);
    if(json.value(sia::tag::UPPERCASE).toBool())
        ui->radioButton_casesensitive_uppercase->setChecked(true);
    else
        ui->radioButton_casesensitive_lowercase->setChecked(true);
    if(json.value(sia::tag::NUMBER_DUH).toInt()== 0)
        ui->radioButton_DUH_answer_yes->setChecked(true);
    else
        ui->radioButton_DUH_answer_no->setChecked(true);
    ui->spinBox_number_try_nak->setValue(json.value(sia::tag::NUMBER_NAK).toInt());
    ui->spinBox_number_try_no_ack->setValue(json.value(sia::tag::NUMBER_NOACK).toInt());
    if(json.value(sia::tag::POLLING).toInt() > 0)
        ui->radioButton_polling_yes->setChecked(true);
    else
    {
        ui->timeEdit_polling_time->setTime(QTime(0,0));
        ui->radioButton_polling_no->setChecked(true);
    }
    ui->timeEdit_polling_time->setTime(QTime::fromMSecsSinceStartOfDay(json.value(sia::tag::POLLING).toInt()*1000));
    ui->timeEdit_trytime_between_no_ack->setTime(QTime::fromMSecsSinceStartOfDay(json.value(sia::tag::TIMEOUT).toInt()*1000));
}

void SiaConfiguration::loadAlarmSpecification(QJsonObject json)
{
    ui->spinBox_alarm_area_precision->setValue(json.value(sia::tag::AREA_PRECISION).toInt());
    ui->spinBox_alarm_feedback_timeout->setValue(json.value(sia::tag::FEEDBACK_TIMEOUT).toInt());
    ui->spinBox_alarm_update_interval->setValue(json.value(sia::tag::ALARM_UPDATE_INTERVAL).toInt());
}
