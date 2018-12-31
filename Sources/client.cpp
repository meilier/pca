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
#include <thread>
#include "common.hpp"
#include "cmessage.hpp"
#include "cqueue.hpp"

#define MYPORT 7000
#define FILEPORT 7001
#define BUFFER_SIZE 1024

int sock_cli;
fd_set rfds;
struct timeval tv;
int retval, maxfd;

//recv message queue
ConcurrentQueue<string> rq;

//send message queue
ConcurrentQueue<string> sq;

//handle message queue
ConcurrentQueue<string> hq;

void receiveProcess()
{
    while (1)
    {
        /*把可读文件描述符的集合清空*/
        FD_ZERO(&rfds);
        maxfd = 0;
        /*把当前连接的文件描述符加入到集合中*/
        FD_SET(sock_cli, &rfds);
        /*找出文件描述符集合中最大的文件描述符*/
        if (maxfd < sock_cli)
            maxfd = sock_cli;
        /*设置超时时间*/
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        /*等待聊天*/
        retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1)
        {
            printf("select出错，客户端程序退出\n");
            break;
        }
        else if (retval == 0)
        {
            printf("客户端没有任何输入信息，并且服务器也没有信息到来，waiting...\n");
            continue;
        }
        else
        {
            /*服务器发来了消息*/

            char recvbuf[BUFFER_SIZE];
            int len;
            len = recv(sock_cli, recvbuf, sizeof(recvbuf), 0);
            // message branch
            printf("%s", recvbuf);
            if (string(recvbuf) == SAR)
            {
                //connect to server file transfer port, such as, 7001
                rq.Push(SAR);
            }
            else if (string(recvbuf) == SAO)
            {
                //
                rq.Push(SAO);
            }
            else if (string(recvbuf) == STR)
            {
                rq.Push(STR);
            }
            else if (string(recvbuf) == STO)
            {
                rq.Push(STO);
            }
            else if (string(recvbuf) == GCR)
            {
                //get certs ready message from server
                //connect to server to get it's certs.tar.gz
                rq.Push(GCR);
            }
            memset(recvbuf, 0, sizeof(recvbuf));
        }
    }
}

void sendProcess()
{
    while (1)
    {
        if (!sq.Empty())
        {
            continue;
        }
        //get message from send queue
        string sqmessage;
        sq.Pop(sqmessage);
        send(sock_cli, sqmessage.c_str(), strlen(sqmessage.c_str()), 0); //send
    }
}

void handleProcess()
{
    while (1)
    {
        if (!rq.Empty())
        {
            continue;
        }
        //get message from queue
        string rpmessage;
        rq.Pop(rpmessage);
        if (rpmessage == SAR)
        {
            //send csr file
            //connect to server file transfer port, such as, 7001
            std::thread t4(fileProcess, 0, 1);
            t4.detach();
        }
        else if (rpmessage == SAO)
        {
            //
            rq.Push(SAO);
        }
        else if (rpmessage == STR)
        {
            rq.Push(STR);
        }
        else if (rpmessage == STO)
        {
            rq.Push(STO);
        }
        else if (rpmessage == GCR)
        {
            //get certs ready message from server
            //connect to server to get it's certs.tar.gz
            rq.Push(GCR);
        }
    }
}

/*********
 * send crs file to server
 * certType: 0 account crs, 1 tls csr.
 * */
void fileProcess(int certType)
{
    int file_cli;

    ///定义sockfd
    file_cli = socket(AF_INET, SOCK_STREAM, 0);
    ///定义sockaddr_in
    struct sockaddr_in fileaddr;
    memset(&fileaddr, 0, sizeof(fileaddr));
    fileaddr.sin_family = AF_INET;
    fileaddr.sin_port = htons(FILEPORT);               ///服务器端口
    fileaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); ///服务器ip

    //连接服务器，成功返回0，错误返回-1
    if (connect(file_cli, (struct sockaddr *)&fileaddr, sizeof(fileaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    //
}

/***********
 * generate client private key and its crs file
 * certType : 0 account , 1 tls.
 * */
void genCert(int certType){

}

int main()
{

    ///定义sockfd
    sock_cli = socket(AF_INET, SOCK_STREAM, 0);
    ///定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);                 ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); ///服务器ip

    //连接服务器，成功返回0，错误返回-1
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    //thread : send
    std::thread t1(sendProcess);
    t1.detach();
    //thread : recv
    std::thread t2(receiveProcess);
    t2.detach();

    //thread : handle
    std::thread t3(handleProcess);
    t3.detach();
    while (1)
    {
    }
    close(sock_cli);
    return 0;
}