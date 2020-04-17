#include <iostream>
#include <chrono>
#include <iomanip>
#include <chrono>

#include "logger.hpp"
#include <ctime>   // localtime

namespace
{
    std::string return_current_time_and_date()
    {
      time_t rawtime;
      struct tm * timeinfo;
      char buffer[80];

      time (&rawtime);
      timeinfo = localtime(&rawtime);

      strftime(buffer,80,"%d-%m-%Y %I:%M:%S ",timeinfo);
      std::string str(buffer);
      return str;
    }
}

namespace eos
{
  namespace  sdk {

    Logger::Logger()
      : screen_output_(false)
    {
    }

    void Logger::log(LogInterface::Verbosity verbosity, std::string const &line)
    {
      std::string content = return_current_time_and_date();

      if (verbosity == LogInterface::Debug)
        content += "[DBG] ";
      else if (verbosity == LogInterface::Info)
        content += "[INF] ";
      else if (verbosity == LogInterface::Warning)
        content += "[WRN] ";

      content += line;

      mutex_.lock();
      if(file_.is_open())
      {
        file_ << content << std::endl;
        file_.flush();
      }
      mutex_.unlock();
      if(screen_output_)
      {
        std::cout<< content << std::endl;
      }
    }

    void Logger::close()
    {
      mutex_.lock();
      file_.close();
      mutex_.unlock();
    }

    void Logger::open(std::string const& filename)
    {
      mutex_.lock();
      if (file_.is_open())
        file_.close();
      file_.open(filename.c_str(), std::ios::app|std::ios::out);
      mutex_.unlock();
    }

    priv::LoggerStream Logger::operator()(LogInterface::Verbosity verbosity)
    {
      return priv::LoggerStream(*this, verbosity);
    }

    priv::LoggerStream::LoggerStream(Logger &logger, LogInterface::Verbosity verbosity)
      : std::ios()
      , std::ostringstream()
      , logger_(logger)
      , verbosity_(verbosity)
    {
    }

    priv::LoggerStream::LoggerStream(priv::LoggerStream const &ls)
      : std::ios()
      , std::ostringstream()
      , logger_(ls.logger_)
      , verbosity_(ls.verbosity_)
    {
      std::ostringstream::str(ls.str());
    }

    priv::LoggerStream::~LoggerStream()
    {
      logger_.log(verbosity_, str());
    }

  } // namespace sdk
} // namespace eos
