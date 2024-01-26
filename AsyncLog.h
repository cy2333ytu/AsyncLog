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
    AsyncLog& operator = (const AsyncLog&) = delete;
    AsyncLog(const AsyncLog&) = delete;
    AsyncLog(): _stop{false}{
        workthread = std::thread([this](){
            for(;;){
                std::unique_lock<std::mutex> lock(_mtx);
                while(_queue.empty() && !_stop){
                    _emptyCond.wait(lock);
                }
                if(_stop){
                    return;
                }
                auto logtask = _queue.front();
                _queue.pop();
                lock.unlock();
                processTask(logtask);
            }
        });
    }
    void processTask(std::shared_ptr<LogTask> task){
        std::cout << "log level is: " << task->_logLevel << std::endl;
        if(task->_logDatas.empty()) return;
        auto head = task->_logDatas.front();
        task->_logDatas.pop();
        
        std::string formatStr = "";
        bool success = convert2Str(head, formatStr);
        if(!success) return;
        
        while(!task->_logDatas.empty()){
            auto data = task->_logDatas.front();
            formatStr = formatString(formatStr, data);
            task->_logDatas.pop();
        }
        std::cout<<"log string is: " << formatStr <<std::endl;

    }
    bool convert2Str(const std::any &data, std::string str){
        std::ostringstream ss;
        if(data.type() == typeid(int)){
            ss << std::any_cast<int>(data);
        }else if(data.type() == typeid(float)){
            ss << std::any_cast<float>(data);
        }else if(data.type() == typeid(double)){
            ss << std::any_cast<double>(data);
        }else if(data.type() == typeid(std::string)){
            ss << std::any_cast<std::string>(data);
        }else if(data.type() == typeid(char*)){
            ss << std::any_cast<char*>(data);
        }else if(data.type() == typeid(char const *)){
            ss << std::any_cast<char const *>(data);
        }else{
            return false;
        }
        str = ss.str();
        return true;
    }
    template<typename ...Args>
    std::string formatString(const std::string& format, Args... args){

    }
    bool _stop;
    std::condition_variable _emptyCond;
    std::queue<std::shared_ptr<LogTask>> _queue;
    std::mutex _mtx;
    std::thread workthread;
};

}
#endif