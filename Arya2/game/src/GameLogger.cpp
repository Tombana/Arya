#include "GameLogger.h"
#include <ctime>
#include <iostream>

//The global object
GameLogger gameLogger;

GameLogger::GameLogger()
{
    stdoutLogLevel = L_INFO | L_WARNING | L_ERROR | L_CRITICALERROR | L_DEBUG;
    fileLogLevel = L_WARNING | L_ERROR | L_CRITICALERROR;
    callbackLogLevel = L_INFO | L_WARNING | L_ERROR | L_CRITICALERROR | L_DEBUG;
    currentLogLevel = L_NONE;
    callbackFunc = nullptr;
}

GameLogger::~GameLogger()
{
    filestream.close();
}

bool GameLogger::setOutputFile(const char* filename, bool append)
{
    using std::ofstream;
    filestream.open(filename, (append ? ofstream::out | ofstream::app : ofstream::out));
    return filestream.is_open();
}

void GameLogger::closeOutputFile()
{
    filestream.close();
}

GameLogger& endLog(GameLogger& logger)
{
    logger.flush();
    return logger;
}

void GameLogger::flush()
{
    if( stdoutLogLevel & currentLogLevel ){
        std::cout << streambuff.str() << std::endl;
    }
    if( fileLogLevel & currentLogLevel ){
        char buf[80];
        std::time_t now = std::time(NULL);
        strftime(buf, sizeof(buf), "[%Y-%m-%d %X] ", std::localtime(&now));
        filestream << buf << streambuff.str() << std::endl;
    }
    if(callbackFunc)
        if(callbackLogLevel & currentLogLevel)
            callbackFunc(streambuff.str());

    currentLogLevel = L_NONE;
    streambuff.str(std::string());
    streambuff.seekp(0);
    streambuff.clear();
}
