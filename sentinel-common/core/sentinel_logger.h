#ifndef SENTINEL_LOGGER_H
#define SENTINEL_LOGGER_H

#include <QString>
#include <QTextStream>
#include <QMutex>
#include <QFile>
#include <sstream>

#include "core/singleton.h"

namespace core
{
    class Logger;
    namespace log
    {
        enum Level
        {
            Debug,
            Info,
            Warning,
            Error
        };
    }

    namespace priv
    {
        class LoggerStream
                : public std::ostringstream
        {
        public:
            LoggerStream(Logger &logger, log::Level level);
            LoggerStream(LoggerStream const &ls);
            ~LoggerStream();

        private:
            Logger &logger_;
            log::Level level_;
        };

    }

    class Logger
            : public Singleton<Logger>
    {
    public:
        Logger();
        void open(QString const& fileName);

        priv::LoggerStream operator()(log::Level level);

    private:
        friend class priv::LoggerStream;

        void log(log::Level level, std::string const &line);
        ///! file output
        QFile file_;
        ///! the output stream
        QTextStream fileStream_;
        QMutex mutex_;
    };

}

#define EOS_Debug() (*core::Logger::instance())(core::log::Debug)
#define EOS_Info() (*core::Logger::instance())(core::log::Info)
#define EOS_Warning() (*core::Logger::instance())(core::log::Warning)
#define EOS_Error() (*core::Logger::instance())(core::log::Error)

#endif // SENTINEL_LOGGER_H
