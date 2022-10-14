#include <iostream>
#include "app_engine.h"
#include <boost/thread.hpp>
#include <chrono>
int main(){
    std::cout<<"hello"<<std::endl;
    GetAppEngine()->Start();
    while(true) {
        boost::this_thread::sleep(boost::posix_time::seconds(5));
    }
    GetAppEngine()->Stop();
    return true;
}