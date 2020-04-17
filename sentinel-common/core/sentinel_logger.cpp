#include <QDateTime>
#include <iostream>
#include <QDebug>

#include "sentinel_logger.h"
#include "core/workspace.h"

namespace core
{
  Logger::Logger()
  {
  }

  void Logger::log(log::Level level, std::string const &line)
  {
    QString content;
    content = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss.zzz") + " ";

    if (level == log::Debug)
      content += "[DBG] ";
    else if (level == log::Info)
      content += "[INF] ";
    else if (level == log::Warning)
      content += "[WRN] ";
    else if (level == log::Error)
      content += "[ERR] ";

    content += line.c_str();

    mutex_.lock();
    fileStream_ << content << "\n";
    fileStream_.flush();
    mutex_.unlock();
    qDebug() << content;
  }

  void Logger::open(QString const &fileName)
  {
    if (file_.isOpen())
      file_.close();
    file_.setFileName(core::join(core::Workspace::local(core::Workspace::LogDay),
                                 fileName));
    if (file_.open(QFile::WriteOnly | QFile::Append) == false)
    {
      throw std::string("Error while opening the log");
    }
    std::cout << "Log file : " << file_.fileName().toStdString() << std::endl;
    fileStream_.setDevice(&file_);
  }

  priv::LoggerStream Logger::operator()(log::Level level)
  {
    return priv::LoggerStream(*this, level);
  }

  priv::LoggerStream::LoggerStream(Logger &logger, log::Level level)
      : std::ios()
      , std::ostringstream()
      , logger_(logger)
      , level_(level)
  {
  }

  priv::LoggerStream::LoggerStream(priv::LoggerStream const &ls)
      : std::ios()
      , std::ostringstream()
      , logger_(ls.logger_)
      , level_(ls.level_)
  {
    std::ostringstream::str(ls.str());
  }

  priv::LoggerStream::~LoggerStream()
  {
    logger_.log(level_, str());
  }
}
