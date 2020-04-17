#include <QDir>

#include "transfer.h"
#include "core/workspace.h"
#include "sftp.h"
#include "core/constants.h"

#include <QFile>
#include <QDir>
#include <QTextStream>

namespace eos
{
  namespace sftp
  {
    namespace
    {
      std::map<eos::Configuration::Model, QString> filenames =
      {
        {eos::Configuration::Model::BaseMap,         eos::constants::shared::BASE_MAP},
        {eos::Configuration::Model::StaticMap,       eos::constants::shared::STATIC_MAP},
        {eos::Configuration::Model::DisplayMap,      eos::constants::shared::DISPLAY_MAP},
        {eos::Configuration::Model::Patrols,         eos::constants::shared::PATROLS},
        {eos::Configuration::Model::Zones,           eos::constants::shared::ZONES},
        {eos::Configuration::Model::Settings,        eos::constants::shared::SETTINGS},
        {eos::Configuration::Model::TasksScheduled,  eos::constants::shared::TASKS_SCHEDULED},
        {eos::Configuration::Model::Sensors,         eos::constants::shared::SENSORS},
      };

      std::map<eos::Configuration::Model, QString> requests =
      {
        {eos::Configuration::Model::BaseMap,         "map"},
        {eos::Configuration::Model::StaticMap,       eos::constants::shared::STATIC_MAP},
        {eos::Configuration::Model::DisplayMap,      eos::constants::shared::DISPLAY_MAP},
        {eos::Configuration::Model::Patrols,         "patrol"        },
        {eos::Configuration::Model::Zones,           "area"          },
        {eos::Configuration::Model::Settings,        "configuration" },
        {eos::Configuration::Model::TasksScheduled,  "task"          },
        {eos::Configuration::Model::Sensors,         "sensor"        },
      };


      QString get_request(eos::Configuration::Model model)
      {
        std::map<eos::Configuration::Model, QString>::const_iterator it = requests.find(model);
        if(it != filenames.end())
          return it->second;
        return "";
      }

      QString get_filename(eos::Configuration::Model model)
      {
        std::map<eos::Configuration::Model, QString>::const_iterator it = filenames.find(model);
        if(it != filenames.end())
          return it->second;
        return "";
      }
    }

    Transfer::Transfer(core::AppInstance & instance)
      : instance_(instance)
      , done_(false)
    {
      sftp_ = std::make_shared<QSftp>();
      temp_dir_.isValid();
      if(sftp_)
      {
          connect(sftp_.get(), SIGNAL(progressing(double)), this, SLOT(progressing(double)));
      }


      connect(this, SIGNAL(define_model(int, int)), this, SLOT(model_used(int, int)));
      connect(this, SIGNAL(define_downloaded(int,bool, QString const&)), this, SLOT(_downloaded(int, bool, QString const&)));
      connect(this, SIGNAL(define_uploaded(int,bool)), this, SLOT(_uploaded(int,bool)));

      QObject::connect(&httpClient_,SIGNAL(onConnectionRefused(void))    ,this,SLOT(onRobotDisconnected(void)));
      QObject::connect(&httpClient_,SIGNAL(onConnected(void))            ,this,SLOT(onRobotConnected(void)));
      QObject::connect(&httpClient_,SIGNAL(onDataAvailable(com::Patrol)) ,this,SLOT(onDataAvailable(com::Patrol)));
      QObject::connect(&httpClient_,SIGNAL(onDataAvailable(com::Area)) ,this,SLOT(onDataAvailable(com::Area)));

      start();
    }

    Transfer::~Transfer()
    {
      done_ = true;
      wait_for_job_.wakeOne();
      wait(1000);
    }

    QString Transfer::getHostUrl(eos::Configuration::Model model)
    {
        int port = instance_.getRestPort();
        QString robotIp = instance_.robotIP();
        QString request_model =  get_request(model);
        QString hostUrl =  "http://"+ robotIp + ":" + QString::number(port) + "/" + request_model;
        return hostUrl;
    }

    void Transfer::download(eos::Configuration::Model model)
    {
        bool isHttp = instance_.getUseHttp();
        if(isHttp)
        {
            httpClient_.Get(getHostUrl(model));
        }
        else
        {
            QMutexLocker loc(&mutex_job_);
            QString filename = get_filename(model);
            if(filename.size())
            {
              jobs_.enqueue(
              {
                      model,
                      Job::Download,
                      QDir::cleanPath(core::Workspace::remote(core::Workspace::SharedDir) + QDir::separator() + filename),
                      QDir::cleanPath(temp_dir_.path() + QDir::separator() + filename),
                    });
              wait_for_job_.wakeOne();
            }
        }
    }

    void Transfer::upload(eos::Configuration::Model model)
    {
        bool isHttp = instance_.getUseHttp();
        if(isHttp)
        {
            QString filename = get_filename(model);
            if(filename.size())
            {
              QString local_file = QDir::cleanPath(temp_dir_.path() + QDir::separator() + filename);
              instance_.configuration().export_model(model, local_file.toStdString());
              // read a file
              QFile file(local_file);
              if(not file.open(QIODevice::ReadOnly))
              {
                qWarning() << "Transfer::upload - Failed to  open file " << local_file;
              }
              else
              {
                  QByteArray data;
                  QTextStream s1(&file);
                  data.append(s1.readAll());
                  //qDebug() << "string content:" << data; // empty!!!
                  qInfo() << "Upload Data Contenent ::::  "<< data;
                    qInfo() << "Data Upload Path " << local_file;
                  if(data.isEmpty())
                  {
                    qWarning() << "Transfer::upload - data is empty " << local_file;
                  }
                  httpClient_.Post(getHostUrl(model), data);
              }

            }
            else
            {
                qWarning() << "Transfer::upload - ABORTED Failed to read model " << static_cast<int>(model);
            }
        }
        else
        {
          QMutexLocker loc(&mutex_job_);
          QString filename = get_filename(model);
          if(filename.size())
          {
            QString local_file = QDir::cleanPath(temp_dir_.path() + QDir::separator() + filename);
            instance_.configuration().export_model(model, local_file.toStdString());
            jobs_.enqueue(
            {
                    model,
                    Job::Upload,
                    QDir::cleanPath(core::Workspace::remote(core::Workspace::SharedDir) + QDir::separator() + filename),
                    local_file,
                  });
            wait_for_job_.wakeOne();
          }
        }
    }

    void Transfer::onConnected()
    {
        return ;
    }
    void Transfer::onConnectionRefused()
    {
        return ;
    }


    bool Transfer::setup_connection()
    {
      if(sftp_)
      {
          int port = instance_.getSslPort();
          sftp_->setParameters(host_, "eos", obfuscation<'R','S','d','4','9','g','m','9'>().c_str(),port);
          if(sftp_->connect() == QSftp::OK)
            return true;
          sftp_->setParameters(host_, "e-vigilante", obfuscation<'R','S','d','4','9','g','m','9'>().c_str(),port);
          if(sftp_->connect() == QSftp::OK)
            return true;
          sftp_->setParameters(host_, "transfert", obfuscation<'j','h','f','b','q','x','8','j','7','2','w','p','9','p','i','a'>().c_str(),port);
          if(sftp_->connect() == QSftp::OK)
            return true;
          sftp_->setParameters(host_, "transfert", obfuscation<'y','7','k','5','q','e','t','d','5','8','s','e','t','g','u','j'>().c_str(),port);
          if(sftp_->connect() == QSftp::OK)
            return true;
      }
      return false;
    }

    void Transfer::onDataAvailable(com::Patrol const& patrol)
    {
        eos::Configuration::Model model = eos::Configuration::Patrols;

        QString filename = QDir::cleanPath(temp_dir_.path() + QDir::separator() + get_filename(model) ) ;
        QString data = patrol.data;

        /// writting
        ///
        QDir dir;
        dir.mkpath(QDir::homePath()+QDir::separator() + core::Workspace::remote(core::Workspace::SharedDir));
        com::BatchFileDir fileDir = QDir::cleanPath(filename);
        QFile file(fileDir);
        if(!file.open(QIODevice::WriteOnly))
        {
            qWarning() << "File existance " <<  file.exists() << "or file open "<< file.open(QIODevice::WriteOnly) ;
            return;
        }

        QByteArray byte = data.toUtf8();

        file.write(byte);
        file.close();


        _downloaded(model,true,filename);
         qInfo() << "  Patrol found  " << patrol.data ;
         qInfo() <<"   saving to; " << filename ;

        return ;
    }
    //Area
    void Transfer::onDataAvailable(com::Area const& area)
    {
        eos::Configuration::Model model = eos::Configuration::Zones;

        QString filename = QDir::cleanPath(temp_dir_.path() + QDir::separator() + get_filename(model) ) ;
        QString data = area.data;

        /// writting
        ///
        QDir dir;
        dir.mkpath(QDir::homePath()+QDir::separator() + core::Workspace::remote(core::Workspace::SharedDir));
        com::BatchFileDir fileDir = QDir::cleanPath(filename);
        QFile file(fileDir);
        if(!file.open(QIODevice::WriteOnly))
        {
            qWarning() << "File existance " <<  file.exists() << "or file open "<< file.open(QIODevice::WriteOnly) ;
            return;
        }

        QByteArray byte = data.toUtf8();

        file.write(byte);
        file.close();


        _downloaded(model,true,filename);
         qInfo() << "  Area found  " << area.data ;
         qInfo() <<"   saving to; " << filename ;

        return ;
    }

    void Transfer::run()
    {
      while(done_ == false)
      {
        while(jobs_.size())
        {
          mutex_job_.lock();
          Job job = jobs_.dequeue();
          mutex_job_.unlock();
          qDebug() << "job : " << job.sens << " " << job.remote_file << " " << job.local_file;
          emit define_model(job.model, job.sens);
          // test the connection
          if(setup_connection() == false)
          {
            // failed to connect, so report that and continue
            emit downloaded(job.model, false);
            continue ;
          }
          if(job.sens == Job::Download)
          {
                     emit define_downloaded(job.model,
                                            sftp_->download(job.remote_file, job.local_file),
                         job.local_file);
          }
          else
          {
            emit define_uploaded(job.model,
                                 sftp_->upload(job.local_file, job.remote_file));
          }
          sftp_->disconnect();
        }
        mutex_wait_job_.lock();
        wait_for_job_.wait(&mutex_wait_job_);
        mutex_wait_job_.unlock();
      }
    }

    void Transfer::hostChanged(QString const& host)
    {
      host_ = host;
    }

    void Transfer::progressing(double p)
    {
      if(sens_ == Job::Download)
        emit downloading(model_, p);
      else
        emit uploading(model_, p);
    }

    void Transfer::model_used(int model, int sens)
    {
      model_ = model;
      sens_ = sens;
    }

    void Transfer::_downloaded(int model, bool ok, QString const& local_file)
    {
      if(ok)
        instance_.configuration().import_model(static_cast<eos::Configuration::Model>(model), local_file.toStdString());
      emit downloaded(model, ok);
    }

    void Transfer::_uploaded(int model, bool ok)
    {
      emit uploaded(model, ok);
    }

  } // namespace ssh
} // namespace eos
