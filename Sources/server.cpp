#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <list>
#include "cert.hpp"

#define PORT 7000
#define FILEPORT 7001
#define IP "127.0.0.1"
#define MAXLINE 4096

// control message form client
enum message {SignAccount,SignTls,Get,Revoke} cMessage;

// for easy mode ,we ues single process
int sema = 1;
int messageSock;
int fileSock;
struct sockaddr_in servaddr;
struct sockaddr_in fileaddr;
socklen_t len;
socklen_t filelen;
std::list<int> li;
Cert *mCert;

void getConn() {
    while(1){
        int conn = accept(messageSock, (struct sockaddr*)&servaddr, &len);
        li.push_back(conn);
        printf("%d\n", conn);
    }
}

void sendCertificate(){
    
}

void getData() {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    while(1) {
        std::list<int>::iterator it;
        for(it=li.begin(); it!=li.end(); ++it){            
            fd_set rfds;    
            FD_ZERO(&rfds);
            int maxfd = 0;
            int retval = 0;
            FD_SET(*it, &rfds);
            if(maxfd < *it){
                maxfd = *it;
            }
            retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
            if(retval == -1){
                printf("select error\n");
            }else if(retval == 0) {
                //printf("not message\n");
            }else{
                char rbuf[1024];
                memset(rbuf, 0 ,sizeof(rbuf));
                int len = recv(*it, rbuf, sizeof(rbuf), 0);
                printf("%s", rbuf);
                //here for a simple test, we use single process
                sema--;
                if(sema <= 0){
                    printf("other nodes signing");
                    char sbuf[1024];
                    strcpy(sbuf,"wait");
                    int len = send(*it, sbuf, sizeof(sbuf), 0);
                    continue;                 
                }
                if (strcmp(rbuf,"sign-accout")){
                    //do get account csr file and sign and return pem
                    printf("start to sign account cert");
                    
                    //new thread to write file , sign , send done message , send pem file
                    //first prepare a listenning socket and accept, then send ok message to 
                    //client for it to send csrfile, when file is complete , start to sign
                    // when sign process is ok, send sign-ok message to client, client start
                    // to listen at a new socket for server to transport file
                    char sbuf[1024];
                    strcpy(sbuf,"ready-sign-account");
                    int len = send(*it, sbuf, sizeof(sbuf), 0);
                }else if (strcmp(rbuf,"sign-tls")){
                    //do get tls csr file and sign and return pem
                    printf("start to sign tls cert");
                    char sbuf[1024];
                    strcpy(sbuf,"ready-sign-tls");
                    int len = send(*it, sbuf, sizeof(sbuf), 0);
                }else if (strcmp(rbuf,"get-certs")){
                    // transport all pem files
                    printf("start to transport pem files to nodes");
                    mCert->getAllCerts();
                }else if (strcmp(rbuf,"invoke-cert")){
                    // transport all pem files
                    printf("start to transport pem files to nodes");
                    mCert->revokeCert();
                }else{
                    printf("wrong message");
                }
            }
        }
        sleep(1);

    }
}

void signProcess(){
    //get message from queue
    if(message == "sign"){
        //if single prot tell main process to transport file
    }else if(){

    }
}

void getFile() {
    printf("======waiting for client's request======\n");
    while(1){
        int connfd = 0;
        int byteNum;
        char  buff[4096];

        if( (connfd = accept(fileSock, (struct sockaddr*)&fileaddr, &filelen)) == -1){
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }
        //write file
        std::ofstream csrfile(mCert->getCertFileName("csr","account"), std::ios::out | std::ios::trunc);
        while(1){
            byteNum = read(connfd, buff, MAXLINE);
            if(byteNum == 0)
                break;
            csrfile.write(buff,byteNum);
        }
        csrfile.close();
        //sign certificate
        mCert->signCert("account");
        //send sign-ok message

    }
}

void sendFile() {
    while(1) {
        char buf[1024];
        fgets(buf, sizeof(buf), stdin);
        //printf("you are send %s", buf);

        // send process get send pem file message, then send file 
        std::list<int>::iterator it;
        for(it=li.begin(); it!=li.end(); ++it){
            send(*it, buf, sizeof(buf), 0);
        }
    }
}

int main() {
    mCert = new Cert();
    //new message socket
    messageSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(IP);
    if(::bind(messageSock, (struct sockaddr* ) &servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(1);
    }
    if(listen(messageSock, 20) == -1) {
        perror("listen");
        exit(1);
    }
    len = sizeof(servaddr);

    //new file socket
    fileSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&fileaddr, 0, sizeof(fileaddr));
    fileaddr.sin_family = AF_INET;
    fileaddr.sin_port = htons(FILEPORT);
    fileaddr.sin_addr.s_addr = inet_addr(IP);
    if(::bind(fileSock, (struct sockaddr* ) &fileaddr, sizeof(fileaddr))==-1) {
        perror("bind");
        exit(1);
    }
    if(listen(fileSock, 20) == -1) {
        perror("listen");
        exit(1);
    }
    len = sizeof(fileaddr);

    //thread : while ==>> accpet
    std::thread t(getConn);
    t.detach();
    //printf("done\n");
    //thread : input ==>> send
    std::thread t1(sendFile);
    t1.detach();
    //thread : recv ==>> show
    std::thread t2(getFile);
    t2.detach();
    while(1){

    }
    return 0;
}