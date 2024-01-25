#ifndef ASYNCLOH_H
#define ASYNCLOH_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <queue>
#include <sstream>
#include <any>
#include <memory>

namespace AsyncLog
{

enum LogLevel
{
    DEBUGS = 0,
    INFO = 1,
    WARN = 2,
    ERRORS = 3,
};
class LogTask
{
public:
    LogTask(){}
    LogTask(const LogTask&& src):
        _logLevel{src._logLevel}, _logDatas{src._logDatas}{}
    LogTask(const LogTask&& src):
        _logLevel{src._logLevel}, _logDatas{std::move(src._logDatas)}{}

private:
    LogLevel _logLevel;
    std::queue<std::any> _logDatas;
    
};

class AsyncLog{
public:
    static AsyncLog& Instance(){
        static AsyncLog instance;
        return instance;
    }
    ~AsyncLog(){

    }
    void stop(){
        _stop = true;
        _emptyCond.notify_one();
    }
private:
    bool _stop;
    std::condition_variable _emptyCond;
    std::mutex _mtx;
    std::thread workthread;
};

}
#endif