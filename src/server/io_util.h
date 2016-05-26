/*
 * =====================================================================================
 *
 *       Filename:  io_util.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月24日 13时14分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _IO_UTIL_H_
#define _IO_UTIL_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h> // strncpy
#include <glog/logging.h>

int Send(int fd,void* buf,size_t len,int flags){
    int bytes_send,bytes_left;
    bytes_left = len;
    char *ptr = (char*)buf;
    // encode first 4 bytes as length to send
    char temp[5];
    sprintf(temp,"%d",(int)len);
again:
    if(send(fd,temp,5,flags) < 0 ){
        if(EINTR == errno || errno == EWOULDBLOCK || errno == EAGAIN){
            goto again;
        }else if(errno == EPIPE){ // the other side of socket has been closed
            return 0;
        }else{
            LOG(ERROR) << "send head :"<< strerror(errno);
            return -1;
        }
    }
    while(bytes_left >0){
        bytes_send = send(fd,ptr,bytes_left,flags);
        if(bytes_send < 0){
            if(errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK){ // a signal occured before any data was transmitted;
                continue;
            }else if(errno==EPIPE && bytes_send == -1){
                return 0;
            }
        }else if(bytes_send == 0){
            break;
        }

        bytes_left -= bytes_send;
        ptr += bytes_send;
    }
    return len-bytes_left;
}

int Recive(int sockfd,void *buf,size_t len,int flags){
    int bytes_recv,bytes_left;
    char *ptr = (char*)buf;
    // parse first 4 bytes as length to recv
    char num[5];
    recv(sockfd,num,5,flags);
    bytes_left = atoi(num);
    while( bytes_left > 0 ){
        bytes_recv = recv(sockfd,ptr,bytes_left,flags);
        if(bytes_recv < 0){
            if(errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK){
                continue;
            }else if(errno==EPIPE && bytes_recv==-1){
                break;
            }
        }else if(bytes_recv == 0){
            break; // no data recived from sockfd
        }
        bytes_left -= bytes_recv;
        ptr += bytes_recv;
    }
    return len-bytes_left;
}

#endif
