#ifndef EVIGILANTE_SDK_LOGGER_HPP
#define EVIGILANTE_SDK_LOGGER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>

#include "log_interface.hpp"
#include "singleton.hpp"


namespace eos
{
    namespace sdk
    {
      class Logger;

      namespace priv
      {
        class LoggerStream
            : public std::ostringstream
        {
          public:
            LoggerStream(Logger &logger, LogInterface::Verbosity verbosity);
            LoggerStream(LoggerStream const &ls);
            ~LoggerStream();

          private:
            Logger &logger_;
            LogInterface::Verbosity verbosity_;
        };

      }

      class Logger
          : public Singleton<Logger>
      {
        public:
          Logger();
          void open(std::string const& filename);

          void close();

          inline void activateScreenOutput(bool enable)
          {
              screen_output_ = enable;
          }

          priv::LoggerStream operator()(LogInterface::Verbosity verbosity);

        private:
          friend class priv::LoggerStream;

          void log(LogInterface::Verbosity verbosity, std::string const &line);
          ///! file output
          std::fstream file_;
          ///! the output stream
          std::mutex mutex_;
          ///! display log on screen ?
          bool screen_output_;
      };


#define LOG_DEBUG(stream) (*eos::sdk::Logger::instance())(eos::sdk::LogInterface::Debug) << stream
#define LOG_INFO(stream) (*eos::sdk::Logger::instance())(eos::sdk::LogInterface::Info) << stream
#define LOG_WARN(stream) (*eos::sdk::Logger::instance())(eos::sdk::LogInterface::Warning) << stream

    } // namespace sdk
} // namespace eos


#endif // EVIGILANTE_SDK_LOGGER_HPP
