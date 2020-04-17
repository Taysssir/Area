/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef HTTPWINDOW_H
#define HTTPWINDOW_H

// C++/Qt
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <tuple>
#include <functional>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
// Module
#include "core/workspace.h"

class QFile;
class QLabel;
class QLineEdit;
class QPushButton;
class QSslError;
class QAuthenticator;
class QNetworkReply;
class QCheckBox;

class HttpNetwork : public QObject
{
    Q_OBJECT

public:
    explicit HttpNetwork(QObject *parent = Q_NULLPTR);


    bool Get(QString const & urlSpec);
    bool Post(QString const& urlSpec,QByteArray data);

    enum class E_ACTION{
        GET,
        POST
    };
signals:
    void onDataAvailable(QByteArray const & data, QUrl const & url);
    void invalidUrl();
    void onConnectionLost();
    void onConnected();

private slots:
    void httpFinished(/*QNetworkReply *reply*/);
    void httpReadyRead();
    void slotAuthenticationRequired(QNetworkReply*,QAuthenticator *);
    bool sendRequest(QString const& urlSpec, HttpNetwork::E_ACTION action, QByteArray data = QByteArray("")) ;
    QNetworkReply*  sending(QNetworkRequest & request, QByteArray data);

private:
    bool isConnected_;
#ifndef QT_NO_SSL
    void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
#endif

private:

    QUrl url;
    QNetworkAccessManager qnam;

    bool httpRequestAborted;
    QMap<QNetworkReply*,QByteArray> buffer_; //<!  beacause Http network can send datas packet by packet
};


namespace com
{
    typedef enum
    {
        UNKNOWN_STATE      ,
        ROBOT_STARTED      ,
        INITIALIZING_STATE ,
        ROBOT_SHUTDOWNED   ,
        PATROL_STARTED     ,
        REMAPPING_SUCCEEDED,
        PATROL_STOPPED     ,
        PATROL_INTERRUPTED ,
        REMAPPING_STARTED  ,
        DISPEL_DOUBT       ,
        ALARM              ,
        DOCKING            ,
        RECHARGING         ,
        BLOCKED            ,
        LOST               ,
        MANUAL             ,
        SPECIFIC_ACTION    ,
        DOCKING_FAILED     ,
        HARDWARE_ISSUES    ,
        EMERGENCY_DOCKING  ,
    }E_RobotState;

    typedef struct
    {
        QString state; // random
    }RobotStatus;

    static QMap<E_RobotState,QString> robotStateToStrMap =
    {
                    { ROBOT_STARTED      ,"SERVICE_INITIALIZED"},
                    { INITIALIZING_STATE ,"INITIALIZING_STATE" },
                    { ROBOT_SHUTDOWNED   ,"ROBOT_SHUTDOWNED"   },
                    { PATROL_STARTED     ,"PATROLLING"         },
                    { REMAPPING_SUCCEEDED,"REMAPPING_SUCCEEDED"},
                    { PATROL_STOPPED     ,"PATROL_STOPPED"     },
                    { PATROL_INTERRUPTED ,"PATROL_INTERRUPTED" },
                    { REMAPPING_STARTED  ,"REMAPPING_STARTED"  },
                    { DISPEL_DOUBT       ,"DISPEL_DOUBT"       },
                    { ALARM              ,"ALARM"              },
                    { DOCKING            ,"DOCKING"            },
                    { RECHARGING         ,"ON_THE_DOCK"        },
                    { BLOCKED            ,"BLOCKED"            },
                    { LOST               ,"LOST"               },
                    { MANUAL             ,"MANUAL"             },
                    { SPECIFIC_ACTION    ,"PATROLLING_BATCH"   },
                    { DOCKING_FAILED     ,"DOCKING_FAILED"     },
                    { HARDWARE_ISSUES    ,"HARDWARE_ISSUES"    },
                    { EMERGENCY_DOCKING  ,"EMERGENCY_DOCKING"  },
    };

    typedef struct
    {
        double theta;
        int x;
        int y;

    }Position;

    typedef struct
    {
        QString name;
        double value;
    }Sensor;

    typedef struct
    {
        double percent;
        int backup_time;
        bool isCharging;
    }Battery;
    typedef struct
    {
        std::vector<Sensor> sensor;
    }Sensors;

    typedef struct
    {
        RobotStatus robot_state;
        int timestamp;
        Position position;
        std::vector<Sensor> sensors;
        Battery battery;
        bool emergency_button;
    }RobotState;

    typedef struct
    {
        QString data;
    }Patrol;

    typedef struct
    {
        QString data;
    }Area;

    typedef struct
    {
        QString name;
        int id;
    }Camera;

    typedef struct
    {
        QString robot_type;
        int port;
        QVector<Camera> cameras;
    }RobotInfo;

    typedef struct
    {
        int current;
        int loc;
        int map;
        int reloc;
    }RobotScores;

    typedef std::vector<std::tuple<float, float>> LaserArray;
    typedef struct
    {
       RobotScores scores;
       LaserArray  laser_positions;
    }RobotMonitoring;

    typedef QStringList BatchResultList;

    static com::Sensor getSensorByName(QString name, QJsonObject& json )
    {
        com::Sensor s;
        s.name = name;
        s.value = json["sensors"].toObject()[name].toString().toDouble();
        return s;
    }

    typedef QString BatchFileDir;
    static void fromJsonObject(QJsonObject & json, com::RobotState& robotState)
    {
        robotState =
        {
            json["robot_state"].toObject()["state"].toString(),
            json.value("timestamp").toString().toInt(),
            json["position"].toObject()["theta"].toString().toDouble(),
            json["position"].toObject()["x"].toString().toInt(),
            json["position"].toObject()["y"].toString().toInt(),
            {
               getSensorByName("/sensors/front/left/us"  ,json),
               getSensorByName("/sensors/front/right/us" ,json),
               getSensorByName("/sensors/back/right/us"  ,json),
               getSensorByName("/sensors/back/left/us"   ,json),
               getSensorByName("/sensors/front/center/distance",json),
               getSensorByName("/sensors/up/temperature" ,json),
               getSensorByName("/sensors/up/humidity"    ,json),
               getSensorByName("emergency_button"        ,json),
            },
            {
                json["battery"].toObject()["percent"].toString().toDouble(),
                json["battery"].toObject()["backup_time"].toString().toInt(),
                json["robot_state"].toObject()["state"].toString() == "ON_THE_DOCK",
            },
            json["sensors"].toObject()["emergency_button"].toString() == "DOWN",
        };
   }
}

namespace RequestType
{
  const char GET_BATCH_RESULT[] = "/get_batch_file";
  const char LIST_BATCH_RESULT[] = "/list_batch_file";
  const char STATE_REQ[] = "/state";
  const char INFO_REQ[] = "/info";
  const char MONITOR_REQ[] = "/monitor";
  const char THERMAL_CAM[] = "/?action=stream";
  const char PATROL_REQ[] = "/patrol";
  const char AREA_REQ[] = "/area";
}

class HttpClient : public QObject
{
    Q_OBJECT

    public:
    HttpClient(QObject* parent = nullptr)
    {
        //network
        QObject::connect(&network,SIGNAL(onDataAvailable(QByteArray,QUrl)),this,SLOT(onDataAvailable(QByteArray,QUrl)));
        QObject::connect(&network,SIGNAL(onConnectionLost(void)),this,SIGNAL(onConnectionRefused(void)));
        QObject::connect(&network,SIGNAL(onConnected(void)),this,SIGNAL(onConnected(void)));

        Q_UNUSED(parent);
    }


private :
    HttpNetwork network;

signals:
    void onConnected();
    void onConnectionRefused();
    void jsonCorrupted();
    void unknownRequest(QString const & request);
    void onDataAvailable(com::RobotState const & rState);
    void onDataAvailable(com::RobotInfo const & rState);
    void onDataAvailable(com::RobotMonitoring const& scores);
    void onDataAvailable(com::BatchResultList const& batch);
    void onDataAvailable(com::BatchFileDir const& batch);
    void onDataAvailable(QImage const& batch);
    void onDataAvailable(com::Patrol const& patrol);
    void onDataAvailable(com::Area const& area);

public slots:
    bool Get(const QString &url)
    {
        return network.Get(url);
    }

    bool Post(const QString &url, const QByteArray &data)
    {
        return network.Post(url,data);
    }

    void onDataAvailable(QByteArray const& data, QUrl const& url)
    {
        auto path = url.path();
        const QString urlPath =  path.mid(path.lastIndexOf("/"));

        if(0 == QString::compare(RequestType::GET_BATCH_RESULT, urlPath))
        {
             auto query = url.query();
             const QString filename = query.mid(query.lastIndexOf("=")+1);
             QDir dir;
             dir.mkpath(QDir::homePath()+QDir::separator() + core::Workspace::remote(core::Workspace::SharedDir));
             com::BatchFileDir fileDir = QDir::cleanPath(QDir::homePath()+QDir::separator() + core::Workspace::remote(core::Workspace::SharedDir) + QDir::separator() + filename);
             QFile file(fileDir);
             if(file.exists() || !file.open(QIODevice::WriteOnly)){return;}
             file.write(data);
             file.close();

             /// temp
             onDataAvailable(fileDir);
             return;
        }
        else if(0 == QString::compare(RequestType::PATROL_REQ , urlPath))
        {
            if(data.contains("result"))
            {
                qInfo() << "POST message Ack " << url ;
                //continue;
            }
            else
            {
                com::Patrol patrol;
                patrol.data = data;
                emit onDataAvailable(patrol);
            }
        }
        //Area
        else if(0 == QString::compare(RequestType::AREA_REQ , urlPath))
        {
            if(data.contains("result"))
            {
                qInfo() << "POST message Ack " << url ;
                //continue;
            }
            else
            {
                com::Area area;
                area.data = data;
                emit onDataAvailable(area);
            }
        }
        //

        QJsonDocument jdoc = QJsonDocument::fromJson(data);
        if(!jdoc.isNull())
        {
            QJsonObject json = jdoc.object();


            if(0 == QString::compare(RequestType::LIST_BATCH_RESULT, urlPath))
            {
                /// fill our class
                com::BatchResultList listBatch;
                QJsonArray jObj = json["batchs"].toArray();
                for(auto key : jObj)
                {
                    listBatch.push_back(key.toString());
                }
                emit onDataAvailable(listBatch);
            }
            else if(0 == QString::compare(RequestType::STATE_REQ, urlPath))
            {
                /// fill our class
                com::RobotState robotState;
                fromJsonObject(json,robotState);

                emit onDataAvailable(robotState);
            }
            else if(0 == QString::compare(RequestType::INFO_REQ, urlPath))
            {
                com::RobotInfo robotInfo;

                robotInfo.robot_type = json.value("robot_type").toString();
                robotInfo.port =  json.value("port").toString().toInt();
                QJsonObject jObj = json["cameras"].toObject();
                for(auto key : jObj.keys())
                {
                    com::Camera cam = {key,jObj[key].toString().toInt()};
                    robotInfo.cameras.push_back(cam);
                }
                emit onDataAvailable(robotInfo);
            }
            else if(0 == QString::compare(RequestType::MONITOR_REQ, urlPath))
            {
                QJsonObject scoresObj = json["scores"].toObject();
                com::RobotScores scores
                {
                    scoresObj["CURRENT_SCORE"].toString().toInt(),
                    scoresObj["LOCALIZATION_SCORE"].toString().toInt(),
                    scoresObj["MAPPING_SCORE"].toString().toInt(),
                    scoresObj["RELOCALIZATION_SCORE"].toString().toInt(),
                };
                if(0==scores.current && 0==scores.loc && 0==scores.map && 0==scores.reloc  )
                {
                    return;
                }
                com::RobotMonitoring rMonitoring;
                rMonitoring.scores = scores;
                // laser positions
                QJsonObject laserObj = json["laser"].toObject();
                for(auto key :laserObj.keys())
                {
                    auto posObj = laserObj[key].toObject();
                    rMonitoring.laser_positions.push_back(std::make_tuple(posObj["x"].toString().toInt(),posObj["y"].toString().toInt()));
                }
                emit onDataAvailable(rMonitoring);
            }
            else
            {
                emit unknownRequest(urlPath);
            }
        }
        else
        {
            emit jsonCorrupted();
        }
    }

};


#endif

