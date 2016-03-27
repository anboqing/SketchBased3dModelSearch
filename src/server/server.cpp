/*
 * =====================================================================================
 *
 *       Filename:  test_server.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月24日 13时33分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <sys/socket.h> // socket listen bind connect
#include <sys/types.h>
#include <sys/wait.h> // wait()
#include <netinet/in.h> // struct sockaddr_in
#include <stdlib.h> // getenv
#include <stdio.h>
#include <signal.h> // signal
#include <sys/epoll.h> // epoll function
#include <fcntl.h> // non-blocking

#include <gflags/gflags.h> // commandline arguments parsing util
#include <glog/logging.h> // logging util 

#include "io_util.h"

#include <vector>

const int BUFSIZE = 1024*10;

DEFINE_int32(listen_port,9999,"Port to be listened");
DEFINE_int32(backlog,1024,"The size of listen queue");

void sigact(int fd){
    close(fd);
    exit(-1);
}
void sig_child(int signo){
    pid_t pid;
    int stat = 0;
    pid = wait(&stat);
    LOG(INFO) << "child " << pid << " terminated";
    return;
}

int setnonblocking(int sockfd){
    if( fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFD,0) | O_NONBLOCK) == -1){
        LOG(ERROR) << "fcntl:" << strerror(errno);
        return -1;
    }
    return 0;
}

void do_use_conn(int);

int main(int argc,char **argv){

    google::ParseCommandLineFlags(&argc,&argv,true);

    int listenfd,iret;
    if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        LOG(ERROR) << "create listen socket error";
        return -1;
    }

    struct sockaddr_in serv_addr;
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(FLAGS_listen_port);

    DLOG(INFO) << " listen port " << htons(FLAGS_listen_port);

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
    // use epoll to driven the event-loop 
    struct epoll_event ev,events[1024];
    int nfds,epollfd;

    // create epoll 
    if((epollfd = epoll_create(backlog)) < 0){
        LOG(ERROR) << "epoll_create:"<<strerror(errno);
        exit(EXIT_FAILURE);
    }

    // let epoll to watch listenfd
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if( epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev) == -1){
        LOG(ERROR) << "epoll_ctl:listenfd" << strerror(errno);
        exit(EXIT_FAILURE);
    }

    for(;;){
        if( (nfds = epoll_wait(epollfd,events,backlog,-1)) == -1 ){
            LOG(ERROR) << "epoll_wait:" << strerror(errno);
            exit(EXIT_FAILURE);
        }
        for( int idx = 0; idx < nfds ; ++idx){
            if( events[idx].data.fd == listenfd ) {
                connfd = accept(listenfd,(struct sockaddr*)&client_addr,&addr_len);
                LOG(INFO) << "a client connected, connfd : "<< connfd; 
                if(connfd < 0){
                    if(errno==EINTR){ // accept is slow syscall,when signal occurs,errno will be set to EINTR, program should be restart accept again
                        continue;
                    }
                    LOG(ERROR) << "accept :" << strerror(errno);
                    exit(EXIT_FAILURE);
                }
                // set the connected client fd as non-blocking and add it to epoll watch list
                setnonblocking(connfd); // if use block connfd, server will block on recive 
                ev.events = EPOLLIN | EPOLLET; // 将触发方式设置为水平触发
                ev.data.fd = connfd;
                if( epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&ev) == -1){
                    LOG(ERROR) << "epoll_ctl:connfd " << strerror(errno);
                    exit(EXIT_FAILURE);
                }
            } else{ 
                do_use_conn(events[idx].data.fd);
            }
        }
        /*
           int childpid;
           if( (childpid = fork()) == 0) { // child process 
           close(listenfd); // close listening socket
           do_use_conn(connfd);
           }
           close(connfd); // parent close connected socket;
           */
    } // end for(;;)
    shutdown(listenfd,SHUT_RDWR);
}

void do_use_conn(int connfd){
    char buf[BUFSIZE];
    memset(buf,0,BUFSIZE);
    // just send back whatever client send in
    Recive(connfd,(void*)buf,BUFSIZE,0);
    Send(connfd,(void*)buf,strlen(buf),0) ;
}

