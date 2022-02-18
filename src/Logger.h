#ifndef LOGGER_H
#define LOGGER_H

// Singleton pattern described here: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern

#include <string>
#include <iostream>
#include <fstream>

class Logger
{
    public:
        static Logger& log(void)
        {
            static Logger   instance;
            return instance;
        };

    private:
        Logger() : debugLevel(ERROR) {};

    public:
        Logger(Logger const &)          = delete;
        void operator=(Logger const &)  = delete;

        enum DebugLevel {
            NONE        = -1,
            ERROR       = 0,
            WARNING     = 1,
            INFO        = 2, 
            DEBUG       = 3
        };

    private:
        DebugLevel      debugLevel;
        std::string     logFileName;
        std::ofstream   logFile;

    public:
        void setDebugLevel(DebugLevel l){
            debugLevel = l;
        };
        void setLogFile(std::string fn){
            logFileName = fn;
            logFile.open(fn);
        }
        void out(DebugLevel l, std::string s, bool prefix = true, bool ret = true) {
            if (l >= debugLevel){
                std::string p_str = "";
                if (prefix){
                    switch(l){
                        case ERROR:   p_str = "ERROR  : "; break;
                        case WARNING: p_str = "WARNING: "; break;
                        case INFO:    p_str = "INFO   : "; break;
                        case DEBUG:   p_str = "DEBUG  : "; break;
                        default: break;
                    }
                }

                std::cerr << p_str << s; 
                if (ret) std::cerr << std::endl;

                if (logFile){
                    logFile << p_str << s;
                    if (ret) logFile << std::endl;
                }
            }
        }
        void error(std::string s){
            out(DebugLevel::ERROR, s);
        }
        void warning(std::string s){
            out(DebugLevel::WARNING, s);
        }
        void info(std::string s){
            out(DebugLevel::INFO, s);
        }
        void debug(std::string s){
            out(DebugLevel::DEBUG, s);
        }

};

#define LOG_ERROR(...)   {                      \
    char s[4096];                               \
    snprintf(s, sizeof(s), __VA_ARGS__);        \
    Logger::log().error(s);                     \
}

#define LOG_WARNING(...)   {                      \
    char s[4096];                               \
    snprintf(s, sizeof(s), __VA_ARGS__);        \
    Logger::log().warning(s);                     \
}

#define LOG_INFO(...)   {                      \
    char s[4096];                               \
    snprintf(s, sizeof(s), __VA_ARGS__);        \
    Logger::log().info(s);                     \
}

#define LOG_DEBUG(...)   {                      \
    char s[4096];                               \
    snprintf(s, sizeof(s), __VA_ARGS__);        \
    Logger::log().debug(s);                     \
}

#endif
