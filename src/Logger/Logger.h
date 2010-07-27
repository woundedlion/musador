#ifndef LOGGER_0777D49A_2E1F_4bc4_8B1E_7FD79440AA48
#define LOGGER_0777D49A_2E1F_4bc4_8B1E_7FD79440AA48

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/lexical_cast.hpp"
#include <sstream>
#include <queue>
#include "Utilities/Util.h"
#include "Utilities/Singleton.h"
#include "Utilities/Property.h"
#include "UI/Console.h"

#define LOG(level) Logging::log(Logging::level,LOG_SENDER)

namespace Musador
{
    namespace Logging
    {
        typedef boost::condition_variable Condition;
        typedef boost::thread Thread;
        typedef boost::mutex Mutex;
        typedef boost::mutex::scoped_lock Guard;

        /// @brief The various log levels available when writing a message to the logs.
        typedef enum 
        {
            Debug = 0x0100 | 'D',
            Info = 0x0200 | 'I',
            Warning = 0x0300 | 'W',
            Error = 0x0400 | 'E',
            Critical = 0x0500 | 'C'
        } LogLevel;

        /// @class LogStatement
        /// @brief A single Log Statement
        class LogStatement
        {
        public:

            /// @brief Constructor.
            /// @param[in] lvl One of the members of the LogLevel enumeration indicating the level of this log message.
            /// @param[in] msg A string containing the actual log message.
            LogStatement(LogLevel lvl, const std::wstring& msg) : lvl(lvl),msg(msg) {}

            /// @brief The LogLevel of this log message.
            LogLevel lvl;

            /// @brief The actual text of this log message.
            std::wstring msg;
        };

        /// @class ILogListener
        /// @brief An interface for classes which accept log messages and write them to some destination.
        class ILogListener
        {
        public:

            /// @brief Destructor.
            virtual ~ILogListener() {}

            /// @brief Deliver a log message to this listener.
            virtual void send(const LogStatement& stmt) = 0;
        };

        /// @class ConsoleLogListener
        /// @brief A log listener which writes messages to the console.
        class ConsoleLogListener : public ILogListener
        {
        public:

            /// @brief Constructor.
            ConsoleLogListener();  

            /// @brief Destructor.
            ~ConsoleLogListener();

            /// @brief Deliver a log message to this listener.
            void send(const LogStatement& stmt);

        private:

            UI::Console console;
            LogLevel curLevel;

            static const UI::Console::TextColor LOG_COLOR_DEBUG = UI::Console::COLOR_BLUE_LO;
            static const UI::Console::TextColor LOG_COLOR_INFO = UI::Console::COLOR_WHITE_LO;
            static const UI::Console::TextColor LOG_COLOR_WARNING = UI::Console::COLOR_CYAN_HI;
            static const UI::Console::TextColor LOG_COLOR_ERROR = UI::Console::COLOR_YELLOW_HI;
            static const UI::Console::TextColor LOG_COLOR_CRITICAL = UI::Console::COLOR_RED_HI;

        };

        /// @class Logger
        /// @brief Singleton logging object.
        class Logger : public Util::Singleton<Logger>
        {
            friend class LogWriter;

        public:

            /// @brief Constructor.
            Logger();

            /// @brief Destructor.
            ~Logger();

            /// @brief Shut down the Logger.
            /// This function stops the logging thread and must be called before exit.
            void shutdown();

            /// @brief Set the minimum log level for which messages are displayed.
            /// Log messages which are below this level will be silently supressed.
            void setLevel(LogLevel lvl);

            /// @brief Get a LogWriter to handle writing a log message.
            /// @param[in] lvl The level of the log message that the returned writer will write.
            /// @returns A LogWriter to handle writing a log message.
            LogWriter operator()(LogLevel lvl);

            /// @brief Get a LogWriter to handle writing a log message.
            /// @param[in] lvl The level of the log message that the returned writer will write.
            /// @param[in] sender A string containing a friendly name for the originator of this log message.
            /// @returns A LogWriter to handle writing a log message.
            LogWriter operator()(LogLevel lvl, const std::string& sender);

            /// @brief Get a LogWriter to handle writing a log message.
            /// @param[in] lvl The level of the log message that the returned writer will write.
            /// @param[in] sender A string containing a friendly name for the originator of this log message.
            /// @returns A LogWriter to handle writing a log message.
            LogWriter operator()(LogLevel lvl, const std::wstring& sender);

        private:

            typedef std::queue<LogStatement> LogStatementQueue;

            void run();
            void send(const LogStatement& stmt);

            Mutex logLock;
            Condition logPending;	
            boost::shared_ptr<LogStatementQueue> logMessages;

            LogLevel level;

            Thread * logThread;
        };

        /// @class LogWriter
        /// @brief Object which collects log message text and sends it to a logger when it goes out of scope.
        class LogWriter
        {
        public:

            /// @brief Constructor.
            /// @param[in] logger Pointer to the logger instance to which completed log messages are eventually sent.
            /// @param[in] lvl The lof level of messages that this LogWriter will write.
            /// @param[in] sender A string containing a friendly name for the originator of this log message.
            LogWriter(Logger * logger, LogLevel lvl, const std::wstring& sender);

            /// @brief Destructor.
            ~LogWriter();

            /// @brief Template function which writes various types to the log stream.
            /// @param[in] msg An object of type T to write to the logs.
            /// @returns A reference to the log stream, allowing operator<<() calls to be chained.
            template <typename T>
            LogWriter& operator<<(const T& msg);

        private:

            Logger * logger;
            std::wstringstream logStream;
            bool active;
            bool silent;
            LogLevel lvl;
        };

        template <typename T>
        inline LogWriter& LogWriter::operator<<(const T& msg)
        {
            if (!this->silent)
            {
                this->active = true;
                this->logStream << msg;
            }
            return (*this);
        }

        template <>
        inline LogWriter& LogWriter::operator<<<char>(const char& msg)
        {
            if (!this->silent)
            {
                this->active = true;
                this->logStream << Util::utf8ToUnicode(msg);
            }
            return (*this);
        }

        template <>
        inline LogWriter& LogWriter::operator<<<std::string>(const std::string& msg)
        {
            if (!this->silent)
            {
                this->active = true;
                this->logStream << Util::utf8ToUnicode(msg);
            }
            return (*this);
        }

        LogWriter log(LogLevel lvl);
        LogWriter log(LogLevel lvl, const std::string& sender);
        LogWriter log(LogLevel lvl, const std::wstring& sender);

        template <typename T>
        inline LogWriter& operator<<(LogWriter& w, const Util::Property<T>& p)
        {
            return w << p.get();
        }

    }
}



#endif