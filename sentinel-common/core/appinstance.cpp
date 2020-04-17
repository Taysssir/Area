#include <QFile>
#include <QDir>
#include <QTemporaryDir>
#include <QDebug>
#include <QVariant>

#include "appinstance.h"
#include "workspace.h"
#include "constants.h"

#include "core/helper.h"
#include "core/sentinel_logger.h"
#include "constants.h"
#include "sentinel-common/users/userloader.h"

namespace core
{
namespace
{
constexpr char LOGIN_NAME[] = "login.name";
constexpr char LOGIN_PASSWORD[] = "login.password";
constexpr char LOGIN_CONFIGURATION[] = "login.configuration";
constexpr char LOGIN_LANGUAGE[] = "login.language";
constexpr char CURRENT_IP[] = "current.ip";
constexpr char IP_VECTOR[] = "vector.ip";
constexpr char NAMES_VECTOR[] = "vector.name";
constexpr char USER_PROPERTIES_FILE[] = "user_profile";
constexpr char DEFAULT_SETTINGS[] = "default_settings";
constexpr char CONFIG_FILEPATH[] = "robot_data";
}

AppInstance::AppInstance()
    : is_connected_(false)
    , is_operator_(false)
    , expert_mode_(false)
{
}

AppInstance::~AppInstance()
{
}

Memento AppInstance::restoreMemento(QString const &appName)
{
    QString mementoLocation = join(Workspace::local(Workspace::AppSettings),
                                   "memento");
    QSettings settings(mementoLocation, QSettings::IniFormat);
    settings.beginGroup(appName);
    memento_.login = settings.value(LOGIN_NAME, "").toString();
    memento_.password = settings.value(LOGIN_PASSWORD, "").toString();
    memento_.configuration = settings.value(LOGIN_CONFIGURATION, "").toString();
    memento_.current_ip = settings.value(CURRENT_IP).toString();
    QString ip_vector = settings.value(IP_VECTOR).toString();
    memento_.ip = ip_vector.split(",",QString::SkipEmptyParts);
    QString name_vector = settings.value(NAMES_VECTOR).toString();
    memento_.names = name_vector.split(",",QString::SkipEmptyParts);
    return memento_;
}

 Memento AppInstance::resetMementoIp(QString const &appName)
 {
    core::Memento memento = restoreMemento(appName);
    memento.ip = QStringList();
    memento.names = QStringList();
    saveMemento(memento, appName);
    return restoreMemento(appName);
 }

 Memento AppInstance::resetMementoUser(QString const &appName)
 {
    core::Memento memento = restoreMemento(appName);
    memento.login = "";
    memento.password = "";
    memento.configuration = "";
    saveMemento(memento, appName);
    return restoreMemento(appName);
 }

 Memento AppInstance::removeIpFromMemento(QString const &appName, QString ip_to_remove)
 {
    core::Memento memento = restoreMemento(appName);
    int index_to_remove = memento.ip.indexOf(ip_to_remove);
    memento.ip.removeAt(index_to_remove);
    memento.names.removeAt(index_to_remove);
    saveMemento(memento, appName);
    return restoreMemento(appName);
 }

void AppInstance::setMemento(Memento const &memento)
{   
    memento_ = memento;
}

void AppInstance::saveMemento(Memento const &memento, QString const &appName)
{
    QString mementoLocation = join(Workspace::local(Workspace::AppSettings),
                                   "memento");
    QSettings settings(mementoLocation, QSettings::IniFormat);
    settings.beginGroup(appName);
    settings.setValue(LOGIN_NAME, memento.login);
    settings.setValue(LOGIN_PASSWORD, memento.password);
    settings.setValue(LOGIN_CONFIGURATION, memento.configuration);
    settings.setValue(CURRENT_IP, memento.current_ip);
    settings.setValue(IP_VECTOR, memento.ip.join(','));
    settings.setValue(NAMES_VECTOR, memento.names.join(','));
}

void AppInstance::loadConfiguration()
{
    QString configuration_file = join(Workspace::local(Workspace::SentinelConfigurations),
                                   robotIP());
    configuration_.open(configuration_file.toStdString());
}

void AppInstance::login(QString const &login)
{
    login_ = login;
}

QString const &AppInstance::login() const
{
    return login_;
}

User const &AppInstance::user() const
{
    return user_;
}

User &AppInstance::user()
{
    return user_;
}

void AppInstance::loadUser()
{
    QString filePath = core::join(core::Workspace::local(core::Workspace::SentinelData),
                                  USER_PROPERTIES_FILE);
    if(!QFile(filePath).exists())
    {
        filePath = core::join(core::Workspace::local(core::Workspace::App),
                                          DEFAULT_SETTINGS);
    }
    EOS_Info()<< "user properties file load : " << filePath.toStdString();
    Users users = UserLoader::load(filePath);
    if (users.size())
    {
        user_ = users.at(0);
    }
}

void AppInstance::saveUser()
{
    QString filePath = core::join(core::Workspace::local(core::Workspace::SentinelData),
                                  USER_PROPERTIES_FILE);

    Users users;
    users.add(user_);
    UserLoader::save(filePath, users);
}

QString const AppInstance::getLocaleDate(QDateTime date_time, QString string_asked) const
{
    return locale_.toString(date_time, string_asked);
}

bool AppInstance::read_config_settings()
{
    QString config_file_absolute_path = core::join(core::Workspace::local(core::Workspace::App), "config.ini");
    if (QFile::exists(config_file_absolute_path) == false)
    {
        EOS_Error() << "config file not exist : " << config_file_absolute_path.toStdString();
        return false;
    }
    config_settings_.reset(new QSettings(config_file_absolute_path, QSettings::IniFormat));
    return true;
}

bool AppInstance::isStartable(QString const& service_name) const
{
  if(config_settings_)
  {
    config_settings_->beginGroup("LAUNCHABLE SERVICE");
    if (config_settings_->group() != "LAUNCHABLE SERVICE")
    {
        config_settings_->endGroup();
        return false;
    }
    bool result = config_settings_->value(QString(service_name), "true").toBool();
    config_settings_->endGroup();
    return result;
  }
  return false;
}

bool AppInstance::get_language(QLocale::Language &locale_language)
{
    config_settings_->beginGroup("SENTINEL PARAMETERS");
    if (config_settings_->group() != "SENTINEL PARAMETERS")
    {
        setLocaleLanguage(QLocale::English);
        config_settings_->endGroup();
        return false;
    }

    QString language = config_settings_->value("LANGUAGE").toString();
    language = language.toLower();
    if(language.contains("en"))
    {
        setLocaleLanguage(QLocale::English);
        locale_language = QLocale::English;
    }
    else if(language.contains("fr"))
    {
        setLocaleLanguage(QLocale::French);
        locale_language = QLocale::French;
    }
    else if(language.contains("de"))
    {
        setLocaleLanguage(QLocale::German);
        locale_language = QLocale::German;
    }
    config_settings_->endGroup();
    return true;
}

void AppInstance::setExpertMode()
{
    config_settings_->beginGroup("SENTINEL PARAMETERS");
    if (config_settings_->group() != "SENTINEL PARAMETERS")
    {
        expert_mode_ = false;
        config_settings_->endGroup();
        return;
    }
    expert_mode_ = config_settings_->value("EXPERT_MODE", false).toBool();
    config_settings_->endGroup();
}

void AppInstance::loadTemperatureRange(double & min, double & max)
{
    config_settings_->beginGroup("SENSORS");
    min = config_settings_->value("MIN_TEMP", 15).toDouble();
    max = config_settings_->value("MAX_TEMP", 25).toDouble();
    config_settings_->endGroup();
}

bool AppInstance::loadLaserShot()
{
    config_settings_->beginGroup("SENSORS");
    bool retvalue = config_settings_->value("LASER_SHOT", false).toBool();
    config_settings_->endGroup();
    return retvalue;
}

void AppInstance::loadHygroRange(double & min, double & max)
{
    config_settings_->beginGroup("SENSORS");
    min = config_settings_->value("MIN_HYGRO", 45).toDouble();
    max = config_settings_->value("MAX_HYGRO", 55).toDouble();
    config_settings_->endGroup();
}

bool AppInstance::get_optical_camera_settings(QString const& service_name, QVariant & value)
{
    config_settings_->beginGroup("OPTICAL CAMERA");
    if (config_settings_->group() != "OPTICAL CAMERA")
    {
        config_settings_->endGroup();
        return false;
    }
    if(value.type() == QVariant::String)
    {
        if (service_name == "URL_AXIS_MEDIA_PLAY_CLIP")
            value = config_settings_->value( service_name,
                                             "http://%1:55480/axis-cgi/playclip.cgi"
                                             "?clip=%2" ).toString();
        else if (service_name == "URL_AXIS_MEDIA_CLIP_LIST")
            value = config_settings_->value( service_name,
                                             "http://%1:55480/axis-cgi/param.cgi"
                                             "?action=list&group=MediaClip" ).toString();
        else if (service_name == "CODEC")
            value = config_settings_->value(service_name,"h264").toString();
        else if (service_name == "URL_THERMIC_CAMERA")
            value = config_settings_->value( service_name,
                                             "http://%1:55430/?action=stream"
                                             "?name=x.mjpg" ).toString();
        else
            value = config_settings_->value(service_name,"").toString();
    }
    else if(value.type() == QVariant::Int)
    {
        if(service_name == "RTSP_FPS")
            value = config_settings_->value(service_name,15).toInt();
        else if(service_name == "RTSP_BACKUP_DURATION")
            value = config_settings_->value(service_name,30).toInt();
        else if(service_name == "BACKUP_VIDEO_MAX_DURATION")
            value = config_settings_->value(service_name,3600).toInt();
        else if (service_name == "WIDTH")
            value = config_settings_->value(service_name,640).toInt();
        else if (service_name == "HEIGHT")
            value = config_settings_->value(service_name,480).toInt();
        else if (service_name == "COMPRESSION")
            value = config_settings_->value(service_name,100).toInt();
        else
            value = config_settings_->value(service_name,1).toInt();
    }
    else if(value.type() == QVariant::Double)
    {
        if(service_name == "TIMEOUT_CONNECTION")
            value = config_settings_->value(service_name,10).toDouble();
        else if(service_name == "TIMEOUT_STREAM_TREATEMENT")
            value = config_settings_->value(service_name,150).toDouble();
    }
    config_settings_->endGroup();
    return true;
}

bool AppInstance::get_video_recorder_settings(QString const& service_name,
                                              QVariant & value)
{
    config_settings_->beginGroup("VIDEO RECORDER");
    if (config_settings_->group() != "VIDEO RECORDER")
    {
        config_settings_->endGroup();
        return false;
    }
    if(value.type() == QVariant::Int)
    {
        if(service_name == "RECORDER_MAX_FILE_DURATION")
            value = config_settings_->value(service_name,3600).toInt();
    }
    config_settings_->endGroup();
    return true;
}

bool AppInstance::get_video_recorder_settings(QString const& service_name,
                                              QList<QTime> & value)
{
    config_settings_->beginGroup("VIDEO RECORDER");
    if (config_settings_->group() != "VIDEO RECORDER")
    {
        config_settings_->endGroup();
        return false;
    }
    if(service_name == "TIME_SLOTS")
    {
        QString string_configini = config_settings_->value(service_name,"").toString();
        string_configini.replace(" ","");

        QStringList time_slots;
        time_slots = string_configini.split("/");

        foreach(const QString& a_time_slot, time_slots)
        {
            if(a_time_slot.contains("-"))
            {
                QString time;
                QStringList list = a_time_slot.split("-");
                if(list.size() >= 2)
                {
                    time = list.takeFirst();
                    bool result;
                    int time_on_minutes = time.toInt(&result);
                    if(result)
                        value.append(QTime(time_on_minutes/60,time_on_minutes
                                           - (time_on_minutes/60)*60));
                    else
                    {
                        qDebug()<< "VIDEO RECORDER : time readability error in "
                                   "the config.ini files !";
                        config_settings_->endGroup();
                        return false;
                    }
                    time = list.takeFirst();
                    time_on_minutes = time.toInt(&result);
                    if(result)
                        value.append(QTime(time_on_minutes/60,time_on_minutes
                                           - (time_on_minutes/60)*60));
                    else
                    {
                        qDebug()<< "VIDEO RECORDER : time readability error in "
                                   "the config.ini files !";
                        config_settings_->endGroup();
                        return false;
                    }
                }
            }
        }
    }
    config_settings_->endGroup();
    return true;
}

QVariant AppInstance::getData(QString name, QVariant defValue)
{
    QVariant data = defValue;

    if(config_settings_ != nullptr)
    {
        config_settings_->beginGroup("SENTINEL PARAMETERS");
        if (config_settings_->group() == "SENTINEL PARAMETERS")
        {
            data = config_settings_->value(name,data);
        }
        config_settings_->endGroup();
    }
    else
    {
        qWarning()<< "NO get data return default";
    }
    return data;
}
} // core
