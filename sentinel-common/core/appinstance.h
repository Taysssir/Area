#ifndef SENTINEL_APPINSTANCE_H
#define SENTINEL_APPINSTANCE_H

#include <QString>
#include <QLocale>
#include <QDateTime>
#include <memory>
#include <QDebug>
#include <QSettings>

#include "eventmanager.h"
#include "iservices.h"
#include "evigilante_configuration/include/eos/configuration/configuration.hpp"
#include "sentinel-common/users/user.h"
#include "core/sentinel_logger.h"

namespace core
{
/**
 * @brief use to keep info about user
 */
struct Memento
{
    QString login;
    QString password;
    QString configuration;
    QStringList ip;
    QStringList names;
    QString current_ip;
};

enum CAMERA_TYPE {
    MAIN,
    SIDE_UP,
    SIDE_MIDDLE,
    SIDE_DOWN,
    THERMIC
};

struct Camera
{
    Camera()
        : id_(0)
        , rtsp_port_(0)
        , http_port_(0)
    {}
    Camera(uint32_t id, uint32_t rtsp_port, uint32_t http_port)
        : id_(id)
        , rtsp_port_(rtsp_port)
        , http_port_(http_port)
    {
    }
    uint32_t id_;
    uint32_t rtsp_port_;
    uint32_t http_port_;
};

class AppInstance final
        : public IServices
{
public:
    AppInstance();
    ~AppInstance();

    template <typename T>
    void launchService(QString const& name, bool force = false)
    {
        if(isStartable(name) || force)
        {
          EOS_Info() << "Starting service [" << name.toStdString() << "] !";
          IServices::launchService<T, AppInstance>(name);
        }
    }

    template <typename T>
    void relaunchService(QString const& name, bool force = false)
    {
        if(isStartable(name))
        {
          T * service = IServices::getService<T>();
          if(service)
          {
             delete service;
             launchService<T>(name, true);
          }
        }
    }

    Memento restoreMemento(QString const &appName);
    void setMemento(Memento const &memento);
    void saveMemento(Memento const &memento, QString const &appName);
    Memento resetMementoIp(QString const &appName);
    Memento resetMementoUser(QString const &appName);
    Memento removeIpFromMemento(QString const &appName, QString ip_to_remove);

    //QString const &configuration() const;

    eos::PatrolsSharedPointer patrols();
    eos::PatrolsSharedPointer const patrols() const;
    eos::ZonesSharedPointer zones();
    eos::ZonesSharedPointer const zones() const;
    eos::SensorsSharedPointer sensors();
    eos::SensorsSharedPointer const sensors() const;
    eos::TasksScheduledSharedPointer tasksScheduled();
    eos::TasksScheduledSharedPointer const tasksScheduled() const;

    void login(QString const &login);

    QString const &login() const;

    inline EventManager &getEventManager()
    {
        return eventManager_;
    }

    template <typename E>
    void emitEvent(std::unique_ptr<E> event)
    {
        getEventManager().template emitEvent<E>(event);
    }

    template <typename E, typename... Args>
    void emitEvent(Args &&... args)
    {
        getEventManager().template emitEvent<E, Args...>(std::forward<Args>(args)...);
    }

    User const &user() const;
    User &user();
    void loadUser();
    void saveUser();

    void loadConfiguration();

    eos::Configuration const &configuration() const
    {
        return configuration_;
    }

    eos::Configuration &configuration()
    {
        return configuration_;
    }

    bool isConnected() const
    {
        return is_connected_;
    }

    void setConnected(bool c)
    {
        is_connected_ = c;
    }

    bool isModeClientOperator() const
    {
        return is_operator_;
    }

    void setModeClientOperator(bool o)
    {
        is_operator_ = o;
    }


    void setRobotIP(QString const &ip)
    {
        ip_ = ip;
    }

    QString const &robotIP() const
    {
        return ip_;
    }

    bool get_language(QLocale::Language &locale_language);
    void setLocaleLanguage(QLocale::Language language)
    {
        this->locale_ = QLocale(language);
    }
    QLocale getLocaleLanguage()
    {
        return locale_;
    }
    void setExpertMode();
    bool getExpertMode()
    {
        return expert_mode_;
    }

    QString const getLocaleDate(QDateTime date_time, QString string_asked) const;
    QString const getLocaleDate(QDate date, QString string_asked) const
    {
        return getLocaleDate(QDateTime(date),string_asked);
    }
    QString const getLocaleDate(QTime time, QString string_asked) const
    {
        return getLocaleDate(QDateTime(QDate(),time),string_asked);
    }

    bool read_config_settings();
    bool isStartable(const QString &service_name) const;
    bool get_optical_camera_settings(QString const& service_name, QVariant & value);
    bool get_video_recorder_settings(QString const& service_name, QVariant & value);
    bool get_video_recorder_settings(QString const& service_name, QList<QTime> & value);
    int  getSslPort() { return getData("SSL_PORT", QVariant(22)).toInt(); }
    int  getRestPort() { return getData("REST_PORT", QVariant(40054)).toInt(); }
    bool getUseHttp() { return getData("USE_HTTP", QVariant(true)).toBool(); }

    /**
     * @brief getDatafrom configuration *.ini file reads parameters name
     * @param name
     * @param defValue
     * @return paraleter's name value if existe default value else
     */
    QVariant getData(QString name, QVariant defValue);

    bool loadLaserShot();

    const core::AppInstance & read_only() const // Get constant AppInstance for const methods calls
    {
        return *this;
    }

    inline void setRobotType(QString const& robot_type)
    {
      robot_type_ = robot_type;
    }

    inline QString robotType() const
    {
      return robot_type_;
    }

    void addRobotCamera(int typeKey, const Camera &cam) { camerasMap_.insert(typeKey, cam); }
    const Camera getRobotCamera(core::CAMERA_TYPE typeKey) { return camerasMap_.value(typeKey); }

    void loadTemperatureRange(double &min,double &max);
    void loadHygroRange(double &min, double &max);
private:
    QString ip_;
//    QString configuration_path_;
    bool is_connected_;
    bool is_operator_;
    //bool createConfiguration();

    QLocale locale_;
    std::unique_ptr<QSettings> config_settings_;
    ///! where the configuration is store
    //QString configurationLocation_;
    //QString appSettingsLocation_;

//    eos::PatrolsSharedPointer patrols_;
//    //eos::MapSharedPointer map_;
//    eos::ZonesSharedPointer zones_;
//    eos::SettingsSharedPointer settings_;
//    // PlanningSharedPointer planification_;
//    eos::SensorsSharedPointer sensors_;
    core::Memento memento_;
    EventManager eventManager_;

    QString login_;
    eos::Configuration configuration_;
    User user_;
    QString robot_type_;

    bool expert_mode_;

    QMap<int, Camera> camerasMap_; // Key is the camera type
};

inline eos::PatrolsSharedPointer AppInstance::patrols()
{
    return configuration_.patrols();
}

inline eos::PatrolsSharedPointer const AppInstance::patrols() const
{
    return configuration_.patrols();
}

inline eos::ZonesSharedPointer AppInstance::zones()
{
    return configuration_.zones();
}

inline eos::ZonesSharedPointer const AppInstance::zones() const
{
    return configuration_.zones();
}

inline eos::SensorsSharedPointer AppInstance::sensors()
{
    return configuration_.sensors();
}

inline eos::SensorsSharedPointer const AppInstance::sensors() const
{
    return configuration_.sensors();
}

inline eos::TasksScheduledSharedPointer AppInstance::tasksScheduled()
{
    return configuration_.tasksScheduled();
}

inline eos::TasksScheduledSharedPointer const AppInstance::tasksScheduled() const
{
    return configuration_.tasksScheduled();
}

template <typename T>
class EventClient
        : public Receiver<T>
{
public:
    EventClient(AppInstance &appInstance)
        : instance_(appInstance)
    {
    }

    template <typename E, typename Receiver>
    void subscribe(Receiver &receiver)
    {
        instance_.getEventManager().template subscribe<E, Receiver>(receiver);
    }

    template <typename E>
    void emitEvent(std::unique_ptr<E> event)
    {
        instance_.getEventManager().template emitEvent<E>(event);
    }

    template <typename E, typename... Args>
    void emitEvent(Args &&... args)
    {
        instance_.getEventManager().template emitEvent<E, Args...>(std::forward<Args>(args)...);
    }

protected:
    core::AppInstance &instance_;
};
}

#endif // SENTINEL_APPINSTANCE_H
