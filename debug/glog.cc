#include <glog/logging.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

void MyFailureFunction(){
    cout << " when meet fatal condition , will quit ! " << endl;
    exit(1);
}

int main(int argc,char** argv){
    // If glog is used to parse the command line
    // google::ParseCommandLine(&argc,&argv,true);
    // init 
    google::InitGoogleLogging(argv[0]);

    // #############################set log destination
    google::LogToStderr();
    //FLAGS_log_dir = "./";
    LOG(INFO) << "Hello glog ! ";
    LOG(WARNING) << "warning ! " ;

    // #########################3 conditional logging MODE
    int valueint = 20;
    LOG_IF(ERROR,valueint>30) << " error ! " ;

    for(int i = 0 ;i<100;i++){
        LOG_EVERY_N(INFO,50) << " log every 50 " << i << "google::COUNTER : " << google::COUNTER ; 
        LOG_IF_EVERY_N(INFO,i<20,5) << "log if i<20 every 5 i : " << i ; 
        LOG_FIRST_N(INFO,2) << " log first 2 " << i ; 
    }
    
    // ################################## debug mode support #############################
    DLOG(INFO) << " debug meesage ! " ;
    int test = 11;
    DLOG_IF(INFO,test>10) << " debug conditional meesagae ! " ;
    DLOG_EVERY_N(INFO,10) << " debug log every n " << google::COUNTER ;

    // ################################# check mode ##########################
     

    // change the defalt FATAL behaviour to my failure function
    google::InstallFailureFunction(&MyFailureFunction);
    LOG(FATAL) << " fatal ! " ;
    cout << " this is not to be output because of the FATAL log ! " << endl;

    return 0;
}

