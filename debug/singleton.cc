
#include <memory>
#include <iostream>

using namespace std;

#include "../include/Singleton.hpp"

class Manager : public abq::Singleton<Manager>{

public:
    void run(){
        std::cout << "run .. \n" ;
    }
private:

};


int main(){
    auto instance = Manager::GetInstance();
    instance->run();
    return 0;
}
