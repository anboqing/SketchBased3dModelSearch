#include <sys/socket.h> // socket listen bind connect
#include <sys/types.h>
#include <sys/wait.h> // wait()
#include <netinet/in.h> // struct sockaddr_in
#include <stdlib.h> // getenv
#include <stdio.h>
#include <signal.h> // signal
#include <sys/epoll.h> // epoll function
#include <fcntl.h> // non-blocking
#include <opencv2/opencv.hpp>
#include <gflags/gflags.h> // commandline arguments parsing util
#include <glog/logging.h> // logging util 

#include "io_util.h"
#include <iostream>
#include <vector>
using namespace std;
// g++ server_.cpp -o test_server -lglog -lgflags `pkg-config --libs opencv`
const int BUFSIZE = 1024*1024*2;

DEFINE_int32(listen_port,9999,"Port to be listened");
DEFINE_int32(backlog,1024,"The size of listen queue");
DEFINE_int32(pool_size,10,"threads num of the thread pool ");

int listenfd;

void sigact(int fd){
    close(listenfd);
    exit(-1);
}
void sig_child(int signo){
    pid_t pid;
    int stat = 0;
    pid = wait(&stat);
    LOG(INFO) << "child " << pid << " terminated with " << signo;
    return;
}

int setnonblocking(int sockfd){
    if( fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFD,0) | O_NONBLOCK) == -1){
        LOG(ERROR) << "fcntl:" << strerror(errno);
        return -1;
    }
    return 0;
}

char buf[BUFSIZE];

int main(int argc,char **argv){

    google::ParseCommandLineFlags(&argc,&argv,true);

    int iret;
    if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        LOG(ERROR) << "create listen socket error";
        return -1;
    }

    struct sockaddr_in serv_addr;
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(FLAGS_listen_port);

    // bind
    if( (iret = bind(listenfd,(const struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0 ){
        LOG(ERROR) << "bind error" << strerror(errno);
        return -1;
    }

    // listen
    int backlog = FLAGS_backlog;
    char *ptr = getenv("LISTENQ");
    if(ptr!=0)
        backlog = atoi(ptr);
    if(listen(listenfd,backlog) < 0){
        LOG(ERROR) << "listen error" << strerror(errno);
        return -1;
    }


    signal(SIGINT,sigact);
    signal(SIGCHLD,sig_child);
    signal(SIGPIPE,SIG_IGN);

    int connfd;
    struct sockaddr_in client_addr;
    socklen_t addr_len;
    pid_t pid;

    for(;;){
        addr_len = sizeof(sockaddr_in);
        if( (connfd = accept(listenfd,(struct sockaddr*)&client_addr,&addr_len)) < 0){
            if(errno == EINTR) continue;
            else LOG(ERROR) << "accept error";
        }

        DLOG(INFO) << "a client connected ";

        if( (pid = fork()) == 0){

            close(listenfd);
            // read image
            recv(connfd,(void*)buf,BUFSIZE,0);   
            // query

            string str(buf);

            istringstream iss(str);
            char* buf_ = new char[str.size()];
            strcpy(buf_,str.c_str());

            vector<uint8_t> data_vec;
            char *pch;
            pch = strtok(buf_," ");
            data_vec.push_back(static_cast<uint8_t>(atoi(pch)));
            while(pch!=NULL){
                uint8_t n = static_cast<uint8_t>(atoi(pch));
                data_vec.push_back(n);
                pch = strtok(NULL," ");
            }
            data_vec.pop_back();
            cout << " data_vec size " << data_vec.size() << endl;
            // convert data_vec to uint8_t array
            uint8_t data[data_vec.size()];
            for(size_t i = 0; i< data_vec.size();++i){
                data[i] = 255-data_vec[i];
            }
            cv::Mat img = cv::Mat(256,256,CV_8UC4,data).clone();
            cv::cvtColor(img,img,CV_RGB2GRAY);
            cv::imshow("test",img);
            cv::waitKey(0);

            sprintf(buf," hello client %d", connfd);
            // return result
            send(connfd,(void*)buf,strlen(buf),0);
            free(buf_);
            close(connfd);
            exit(0); 
        }
        close(connfd);
    }
}


