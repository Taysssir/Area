#ifndef SIACONFIGURATION_H
#define SIACONFIGURATION_H

#include <QWidget>
#include <QJsonObject>
#include "core/appinstance.h"

namespace Ui {
class SiaConfiguration;
}

class SiaConfiguration : public QWidget
                , public core::EventClient<SiaConfiguration>
{
    Q_OBJECT

public:
    explicit SiaConfiguration(core::AppInstance & instance, QWidget *parent = 0);
    ~SiaConfiguration();

    enum Type_server {
        PRIMARY_SERVER = 0,
        SECONDARY_SERVER = 1
    };
private slots:
    void on_pushButtonSave_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::SiaConfiguration *ui;
    void saveTransmitterInformation(QJsonObject &json);
    void saveServerInformation(QJsonObject &json, Type_server type);
    void saveMessageParameters(QJsonObject &json);
    void saveAlarmSpecification(QJsonObject &json);
    void setupSiaData();
    void loadTransmitters(QJsonObject json);
    void loadServer(QJsonObject json, Type_server type);
    void loadMessage(QJsonObject json);
    void loadAlarmSpecification(QJsonObject json);
};

#endif // SIACONFIGURATION_H
