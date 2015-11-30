#ifndef __SINGLETON_H__
#define __SINGLETON_H__

/*
 * class for implement singleton pattern quickly
 */

/*

 version 0.0 class Singleton{ public:
    static Singleton* GetInstance(){
        if( 0 == _instance){
            _instance = new Singleton; 
        }
        return _instance;
    }

private:
    Singleton(){}
    ~Singleton(){}

    static Singleton* _instance;
};

*/


/* with auto memory management 

#include <memory>
class SingleTon{
public:
    static std::shared_ptr<SingleTon> GetInstance(){
        if(_instance.get()==0){
            _instance.reset(new SingleTon);
        }
        return _instance;
    }
private:
    SingleTon(){}
    ~SingleTon(){}
    static std::shared_ptr<SingleTon> _instance;
};

 */

/* add template support */

#include <memory>
#include <mutex>

namespace abq{

template <typename T>
class Singleton{
    public:
        static inline std::shared_ptr<T> GetInstance();
    protected:
        Singleton(){}
        ~Singleton(){}
    private:
        Singleton(const Singleton&){}
        Singleton& operator=(const Singleton&){}

        static std::shared_ptr<T> _instance;

       static void setInstance(T& t){
            _instance.reset(t);
        }

 //       static std::mutex _mutex;
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance=0; // static member need declared before use
//template <typename T>
//std::mutex Singleton<T>::_mutex;


template <typename T>
inline std::shared_ptr<T> Singleton<T>::GetInstance(){
    std::once_flag instance_created;
//    if(0 ==_instance.get()){
        //std::lock_guard<std::mutex> guard(_mutex);
            std::call_once(instance_created,Singleton::setInstance,new T);
 //   }
    return _instance;
}

}

#endif
