/*
 * =====================================================================================
 *
 *       Filename:  test_client.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月24日 11时35分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <sys/socket.h> // socket
#include <sys/types.h> 
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_pton

#include <signal.h>
#include <unistd.h> // read/ write
#include <stdlib.h>
#include <string.h> // strlen / memset
#include <stdio.h>


#include <gflags/gflags.h> // command line arguments processing
#include <glog/logging.h> // logging processing

#include "io_util.h"

const int BUFSIZE = 1024*10;

DEFINE_string(server_ip,"127.0.0.1","IP address of server eg. 127.0.0.1");
DEFINE_int32(server_port,9999,"Port number of server application eg. 80");

int Send(int,void*,size_t,int);
int Recive(int,void*,size_t,int);

int main(int argc,char **argv){

    google::ParseCommandLineFlags(&argc,&argv,true);
    //FLAGS_log_dir = "./";
    // create socket 
    int sockfd;
    if( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        LOG(FATAL) << "create socket failed " << strerror(errno);
    }

    // prepair server address and port
    struct sockaddr_in serv_addr;
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(FLAGS_server_port);
    if(inet_pton(AF_INET,FLAGS_server_ip.c_str(),&serv_addr.sin_addr) <= 0){
        LOG(ERROR) << "inet_pton error: " << strerror(errno);
        return -1;
    }

    signal(SIGPIPE,SIG_IGN);

    // connet to server
    if( connect(sockfd,(const struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0 ){
        LOG(ERROR) << "connect error: " << strerror(errno);
        return -1;
    }  
    // send/read message message
    //char *buf = (char*)malloc(sizeof(BUFSIZE+1));
    char buf[BUFSIZE+1] = {0};
    for(int i=0;i<10;i++){
        memset((void*)buf,96,BUFSIZE);
        // send message in buf to server
        DLOG(INFO) << " sending message : " << buf;
        ssize_t ret;
        if( (ret = Send(sockfd,(void*)buf,strlen(buf),0)) < 0) {
            LOG(ERROR) <<"send error" << strerror(errno);
            break;
        }

        DLOG(INFO) << " waiting server reply...";
        // recive message from server
        memset((void*)buf,0,BUFSIZE+1);
        if( (ret = Recive(sockfd,(void*)buf,BUFSIZE,0)) < 0 ){
            LOG(ERROR)<<"revive message error" << strerror(errno);
            break;
        }
        printf(" server reply: %ld bytes \n",strlen(buf));
    }
    close(sockfd);
}

