//
// Usage:
// LogError << "Message. Info: " << myvariable << " more info" << endLog;
//

#pragma once

#include <functional>
#include <string>
#include <sstream>
#include <fstream>
#include <glm/glm.hpp>

using glm::vec3;
using glm::vec4;

#define GameLogError    (gameLogger << GameLogger::L_ERROR)
#define GameLogWarning  (gameLogger << GameLogger::L_WARNING)
#define GameLogInfo     (gameLogger << GameLogger::L_INFO)
#define GameLogDebug    (gameLogger << GameLogger::L_DEBUG)

class GameLogger
{
    public:
        GameLogger();
        ~GameLogger();

        enum LOGLEVEL{ //Must be powers of 2 to allow combining
            L_NONE = 0,
            L_DEBUG = 1,
            L_INFO = 2,
            L_WARNING = 4,
            L_ERROR = 8,
            L_CRITICALERROR = 16
        };


        bool setOutputFile(const char* filename, bool append = true);
        void closeOutputFile();

        //! combine the LOGLEVEL flags to specify
        //! which log messages show up at that place
        int stdoutLogLevel;
        int fileLogLevel;
        int callbackLogLevel;

        void setLoggerCallback(std::function<void(const std::string&)> func){ callbackFunc = func; return; }

        //TODO: If this does not compile, revert to old method
        //This might not compile on MSVC++
        template<class T>
            inline GameLogger& operator<<(const T& t) {
                streambuff << t;
                return *this;
            }

    private:
        std::function<void(const std::string&)> callbackFunc;
        std::ofstream filestream;

        //TODO: THIS SHOULD BE THREAD-LOCAL
        std::stringstream streambuff;
        LOGLEVEL currentLogLevel; //log level of the message in the streambuff

        void flush();

        friend GameLogger& endLog(GameLogger& logger);
        friend GameLogger& operator<<(GameLogger& logger, LOGLEVEL lvl);
};

//So you can do logger << endLog;
GameLogger& endLog(GameLogger& logger);

inline GameLogger& operator<<(GameLogger& logger, GameLogger& (*manipulatorFunc)(GameLogger&) ){
    return manipulatorFunc(logger);
};

inline GameLogger& operator<<(GameLogger& logger, GameLogger::LOGLEVEL lvl){
    //This construction allows the following:
    //LogWarning << "test1";
    //LogWarning << "test2" << endLog;
    //so that Warning is only displayed once
    if( logger.currentLogLevel == GameLogger::L_NONE ) {
        logger.currentLogLevel = lvl;
        //Reset the stringstream
        logger.streambuff.str(std::string());
        logger.streambuff.seekp(0);
        logger.streambuff.clear();
        switch(lvl){
            case GameLogger::L_CRITICALERROR:
                logger.streambuff << "Critical ERROR: ";
                break;
            case GameLogger::L_ERROR:
                logger.streambuff << "ERROR: ";
                break;
            case GameLogger::L_WARNING:
                logger.streambuff << "Warning: ";
                break;
            case GameLogger::L_INFO:
                logger.streambuff << "Info: ";
                break;
            case GameLogger::L_DEBUG:
                logger.streambuff << "Debug: ";
                break;
            default:
                break;
        };
    }
    return logger;
}

inline GameLogger& operator<<(GameLogger& logger, const vec3& v)
{
    logger << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return logger;
}

inline GameLogger& operator<<(GameLogger& logger, const vec4& v)
{
    logger << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')';
    return logger;
}


//All << operations that stringstream can do:
//template<class T>
//    inline GameLogger& operator<<(GameLogger& logger, const T& t){
//        logger.getStream() << t;
//        return logger;
//    }

extern GameLogger gameLogger;

