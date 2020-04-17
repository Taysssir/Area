#ifndef SENTINEL_ADMIN_TRANSFER_H
#define SENTINEL_ADMIN_TRANSFER_H

#include <QObject>
#include <QQueue>
#include <QTemporaryDir>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QWaitCondition>

#include "eos/configuration/configuration.hpp"
#include "core/appinstance.h"

#include "http/network.h"

namespace eos
{
  namespace sftp
  {

    class QSftp;

    class Transfer
        : public QThread
    {
      Q_OBJECT
    public:
      Transfer(core::AppInstance & instance);
      ~Transfer();
      void download(eos::Configuration::Model model);
      void upload(eos::Configuration::Model model);
      void hostChanged(QString const& host);
    signals:
      void downloading(int model, double progression);
      void uploading(int model, double progression);
      void downloaded(int model, bool ok);
      void uploaded(int model, bool ok);
    private:
      struct Job
      {
        typedef enum
        {
          Download,
          Upload
        } Sens;
        eos::Configuration::Model model;
        Sens sens;
        QString remote_file;
        QString local_file;
        Job(eos::Configuration::Model model, Sens sens, QString const& remote_file, QString const& local_file)
          : model(model)
          , sens(sens)
          , remote_file(remote_file)
          , local_file(local_file)
        {
        }
      };
    public slots:
      void progressing(double p);
      void model_used(int model, int sens);
      void _downloaded(int model, bool ok, QString const&);
      void _uploaded(int model, bool ok);      
      void onDataAvailable(com::Patrol const& patrol);
      void onDataAvailable(com::Area const& area);
      void onRobotDisconnected() {/* DO SOMETHING */}
      void onRobotConnected() {/* DO SOMETHING */}
      void onConnected();
      void onConnectionRefused();

    signals:
      void define_model(int model, int sens);
      void define_downloaded(int model, bool ok, QString const&);
      void define_uploaded(int model, bool ok);
      void signal_robotDisconnected();

    protected:

      bool setup_connection();

      QString getHostUrl(Configuration::Model model);

      void run();

    private:
      HttpClient httpClient_;
      core::AppInstance &instance_;
      std::shared_ptr<QSftp> sftp_;
      QQueue<Job> jobs_;
      QTemporaryDir temp_dir_;
      QMutex mutex_job_;
      QWaitCondition wait_for_job_; // like 3 506 600 french people :(
      QMutex mutex_wait_job_;
      bool done_;
      int model_;
      int sens_;
      QString host_;
    };

  } // namespace ssh
} // namespace eos

#endif // SENTINEL_ADMIN_TRANSFER_H

