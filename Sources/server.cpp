#ifdef __linux__
#include <signal.h>
#endif
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
#include <map>
#include "cert.hpp"
#include "cqueue.hpp"
#include "cmessage.hpp"

#define PORT 7000
#define FILEPORT 7001
#define IP "127.0.0.1"
#define MAXLINE 4096

typedef struct
{
    int conn;
    string message;
} CInstance;

void getConn();
void fileProcess(int transType, int certType, int conn);
void receiveProcess();
void handleRqProcess();
void handleHqProcess();
void sendProcess();
void sig_handler(int sig);

// for easy mode ,we use single process
int sema = 1;
int messageSock;
int fileSock;
struct sockaddr_in servaddr;
struct sockaddr_in fileaddr;
socklen_t len;
socklen_t filelen;

std::list<int> li;
Cert *mCert;
map<string, string> qqq;

//recv message queue
ConcurrentQueue<CInstance> rq;

//send message queue
ConcurrentQueue<CInstance> sq;

//handle message queue
ConcurrentQueue<CInstance> hq;

// ctrl + c interupt
static volatile int keepRunning = 1;

//getConn mutex condition_variable for non-loop
mutex mtx;
condition_variable cv;

void sig_handler(int sig)
{
    if (sig == SIGINT)
    {
        keepRunning = 0;
    }
}

void getConn()
{
    while (1)
    {
        //here we may tell the other client that some client is connecting
        printf("waiting for client\n");
        if (sema > 0)
        {
            printf("start message listening thread at 7000\n");
            int conn = accept(messageSock, (struct sockaddr *)&servaddr, &len);
            li.push_back(conn);
            for (list<int>::iterator it = li.begin(); it != li.end(); ++it)
                cout << ' ' << *it << endl;
            printf("getConn: the connect fd is %d\n", conn);
            sema--;
            mCert->increaseSerial();
        }
        std::unique_lock<std::mutex> lock(mtx);
        while (sema <= 0)
        {
            printf("blocking for client\n");
            cv.wait(lock);
        }
        //printf("still loop\n");
    }
}

/*******
 * receiveProcess(): receive messag from client
 * and send it to rq -- receive queue, waiting handleProcess to cope with it
 * ***/
void receiveProcess()
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    printf("start receive thread\n");
    while (1)
    {
        std::list<int>::iterator it;
        printf("receiveProcess: wait client to send message\n");
        for (it = li.begin(); it != li.end();)
        {
            printf("receiveProcess: into for loop\n");
            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = 0;
            int retval = 0;
            FD_SET(*it, &rfds);
            if (maxfd < *it)
            {
                maxfd = *it;
            }
            retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
            if (retval == -1)
            {
                printf("select error\n");
            }
            else if (retval == 0)
            {
                //printf("not message\n");
            }
            else
            {
                char rbuf[1024];
                CInstance mCInstance;
                mCInstance.conn = *it;
                printf("receiveProcess: mCInstance conn is %d\n",*it);
                memset(rbuf, 0, sizeof(rbuf));
                int len = recv(*it, rbuf, sizeof(rbuf), 0);
                printf("reveiceProcess: the message is %s\n", rbuf);
                //detect if socket has closed
                if (len == 0)
                {
                    if (errno != EINTR)
                        sema++;
                    std::lock_guard<std::mutex> lock(mtx);
                    close(*it);
                    li.erase(it++);
                    cv.notify_one();
                    continue;
                }

                if (string(rbuf) == SA.c_str())
                {
                    //do get account csr file and sign and return pem
                    printf("start to sign account cert\n");
                    mCInstance.message = SA;
                    //new thread to write file , sign , send done message , send pem file
                    //first prepare a listenning socket and accept, then send ok message to
                    //client for it to send csrfile, when file is complete , start to sign
                    // when sign process is ok, send sign-ok message to client, client start
                    // to listen at a new socket for server to transport file
                    // char sbuf[1024];
                    // strcpy(sbuf,"ready-sign-account");
                    // int len = send(*it, sbuf, sizeof(sbuf), 0);
                    rq.Push(mCInstance);
                }
                else if (string(rbuf) == ST.c_str())
                {
                    //do get tls csr file and sign and return pem
                    printf("rbuf is fule %s\n", rbuf);
                    printf("start to sign tls cert\n");
                    mCInstance.message = ST;
                    // char sbuf[1024];
                    // strcpy(sbuf,"ready-sign-tls");
                    // int len = send(*it, sbuf, sizeof(sbuf), 0);
                    rq.Push(mCInstance);
                }
                else if (string(rbuf) == GC.c_str())
                {
                    // transport all pem files
                    printf("start to transport pem files to nodes\n");
                    mCInstance.message = ST;
                    // mCert->getAllCerts();
                    rq.Push(mCInstance);
                }
                else if (string(rbuf) == GRL.c_str())
                {
                    // transport all pem files
                    printf("start to transport cert revocation list file to nodes\n");
                    mCInstance.message = GRL;
                    // mCert->getAllCerts();
                    rq.Push(mCInstance);
                }
                else if (string(rbuf) == RC.c_str())
                {
                    // transport all pem files
                    printf("start to transport pem files to nodes\n");
                    mCInstance.message = RC;
                    // mCert->revokeCert();
                    rq.Push(mCInstance);
                }
                else
                {
                    printf("wrong message\n");
                }
            }
            it++;
        }
        sleep(1);
    }
}

/*******
 * handleRqProcess(): handle message in rq-- receive, and send result to sq -- sendqueue.
 * */
void handleRqProcess()
{
    printf("start handle receive queue thread\n");
    while (1)
    {

        //get message from queue
        CInstance rpmessage;
        CInstance sqmessage;
        rq.Pop(rpmessage);
        sqmessage.conn = rpmessage.conn;
        if (rpmessage.message == SA)
        {
            //if single port tell main process to transport file
            //main process ready to receive csr file
            std::thread t4(fileProcess, 0, 0, rpmessage.conn);
            t4.detach();
            printf("handleProcess:why can not be here\n");
            sqmessage.message = SAR;
            sq.Push(sqmessage);

            //send sign-ok message
        }
        else if (rpmessage.message == ST)
        {
            //receive tls crs file
            std::thread t4(fileProcess, 0, 1, rpmessage.conn);
            t4.detach();
            sqmessage.message = STR;
            sq.Push(sqmessage);
        }
        else if (rpmessage.message == GC)
        {
            //send certs.tar.gz to client
            mCert->getAllCerts();
            std::thread t4(fileProcess, 1, 2, rpmessage.conn);
            t4.detach();
            sqmessage.message = GCR;
            sq.Push(sqmessage);
        }
        else if (rpmessage.message == GRL)
        {
            //send crl to client
            std::thread t4(fileProcess, 1, 3, rpmessage.conn);
            t4.detach();
            sqmessage.message = GRLR;
            sq.Push(sqmessage);
        }
        else if (rpmessage.message == RC)
        {
            //invoke this client account and tls cert
            mCert->revokeCert();
        }
        else
        {
            printf("wrong message");
        }
    }
}

/*******
 * handleHqProcess(): handle message in hq-- receive, and send result to sq -- sendqueue.
 * */
void handleHqProcess()
{
    printf("start handle handle queue thread\n");
    while (1)
    {

        CInstance hqmessage;
        CInstance sqmessage;
        hq.Pop(hqmessage);
        sqmessage.conn = hqmessage.conn;
        if (hqmessage.message == GACO)
        {
            //sign account certificate
            mCert->signCert("account");
            std::thread t4(fileProcess, 1, 0, hqmessage.conn);
            t4.detach();
            sqmessage.message = SAO;
            sq.Push(sqmessage);
        }
        else if (hqmessage.message == GTCO)
        {
            //sign tls certificate
            mCert->signCert("tls");
            std::thread t4(fileProcess, 1, 1, hqmessage.conn);
            t4.detach();
            sqmessage.message = STO;
            sq.Push(sqmessage);
        }
    }
}
/**********
 * fileProcess recv or send file from or to client
 * transType: 0 get file from client . certType: 0 get account csr, 1 get tls csr
 * transType: 1 send file to client. certType: 1 send file to client, 0 send account pem, 1 send tls pem , 2 send certs.tar.gz to client, 3 send crl file to client
 * */
void fileProcess(int transType, int certType, int conn)
{

    printf("======waiting for client's request======\n");
    if (transType == 0)
    {
        //get from client
        while (1)
        {
            int connfd = 0;
            int byteNum;
            char buff[MAXLINE];
            CInstance hqmessage;
            hqmessage.conn = conn;
            printf("fileProcess: start file transfer listening at 7001\n");
            if ((connfd = accept(fileSock, (struct sockaddr *)&fileaddr, &filelen)) == -1)
            {
                printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
                continue;
            }
            printf("fileProcess: get file connection from client\n");
            //write file
            std::ofstream csrfile;
            if (certType == 0)
            {
                csrfile.open(mCert->getCertFileName("csr", "account"), std::ios::out | std::ios::trunc);
            }
            else
            {
                csrfile.open(mCert->getCertFileName("csr", "tls"), std::ios::out | std::ios::trunc);
            }
            while (1)
            {
                byteNum = read(connfd, buff, MAXLINE);
                printf("fileProcess: why thead not return %d\n", byteNum);
                if (byteNum == 0)
                {
                    close(connfd);
                    csrfile.close();
                    //send file get ok message to handle process
                    if (certType == 0)
                    {
                        hqmessage.message = GACO;
                        hq.Push(hqmessage);
                    }
                    else
                    {
                        hqmessage.message = GTCO;
                        hq.Push(hqmessage);
                    }
                    printf("should be ready to return\n");
                    return;
                }
                csrfile.write(buff, byteNum);
            }
        }
    }
    else
    {
        //send to client
        while (1)
        {
            int connfd = 0;
            int byteNum;
            char buff[4096];
            int readLen = 0;
            try
            {
                if ((connfd = accept(fileSock, (struct sockaddr *)&fileaddr, &filelen)) == -1)
                {
                    printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
                    continue;
                }
            }
            catch (exception e)
            {
                cout << "execption aaaaa !!!!!" << endl;
                cout << e.what() << endl;
            }
            //open file
            ifstream sfile;
            if (certType == 0)
            {
                //open account pem file
                sfile.open(mCert->getCertFileName("pem", "account"), ios::out | ios::in);
            }
            else if (certType == 1)
            {
                //open tls pem file
                sfile.open(mCert->getCertFileName("pem", "tls"), ios::out | ios::in);
            }
            else if (certType == 2)
            {
                //open tar.gz file
                sfile.open(mCert->getCertFileName("compact"), ios::out | ios::in);
            }
            else if (certType == 3)
            {
                //open csrfile file
                sfile.open(mCert->getCertFileName("crl"), ios::out | ios::in);
            }
            while (!sfile.eof())
            {
                printf("fileProcess:ready to send pem file\n");
                sfile.read(buff, sizeof(buff));
                //printf("buff is %s\n",buff);
                readLen = sfile.gcount();
                send(connfd, buff, readLen, 0);
                printf("fileProcess:here1\n");
            }
            printf("fileProcess:here2\n");
            close(connfd);
            sfile.close();
            //sq.Push("SPO");
            //send file get ok message to handle process
            //may be here, client should send get pem file ok message, otherwise we send it again
            return;
        }
    }
}

void sendProcess()
{
    printf("start send thread\n");
    while (1)
    {

        // char buf[1024];
        // fgets(buf, sizeof(buf), stdin);
        // //printf("you are send %s", buf);

        // // send process get send pem file message, then send file
        // std::list<int>::iterator it;
        // for(it=li.begin(); it!=li.end(); ++it){
        //     send(*it, buf, sizeof(buf), 0);
        // }
        CInstance sqmessage;
        sq.Pop(sqmessage);
        printf("sendProcess: conn is %d get message %s\n", sqmessage.conn, sqmessage.message.c_str());
        // std::list<int>::iterator it;
        // it = li.begin();
        int connfd = sqmessage.conn;
        if (sqmessage.message == SAR)
        {
            //get csr file
            const char *c_s = SAR.c_str();
            char ff[11];
            printf("sendProcess:whyaaaaaaaa %s  %d  %d %d %d \n", SAR.c_str(), sizeof(*SAR.c_str()), sizeof(c_s), sizeof("ssssssssss"), sizeof(ff));
            send(connfd, SAR.c_str(), SAR.length(), 0);
        }
        else if (sqmessage.message == SAO)
        {
            //ready to tranport pem to client
            send(connfd, SAO.c_str(), SAO.length(), 0);
        }
        else if (sqmessage.message == STR)
        {
            //get csr file
            send(connfd, STR.c_str(), STR.length(), 0);
        }
        else if (sqmessage.message == STO)
        {
            //ready to tranport pem to client
            send(connfd, STO.c_str(), STO.length(), 0);
        }
        else if (sqmessage.message == GCR)
        {
            send(connfd, GCR.c_str(), GCR.length(), 0);
        }
        else if (sqmessage.message == GRLR)
        {
            send(connfd, GRLR.c_str(), GRLR.length(), 0);
        }
        else
        {
            printf("wrong send queue message");
        }
    }
}

int main()
{
    mCert = &mCert->getInstance();
    signal(SIGINT, sig_handler);
    //new message socket
    messageSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(IP);
    if (::bind(messageSock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    if (listen(messageSock, 20) == -1)
    {
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
    if (::bind(fileSock, (struct sockaddr *)&fileaddr, sizeof(fileaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    if (listen(fileSock, 20) == -1)
    {
        perror("listen");
        exit(1);
    }
    len = sizeof(fileaddr);

    //thread : getconnection from client
    std::thread t(getConn);
    t.detach();
    printf("start get\n");
    //thread : send
    std::thread t1(sendProcess);
    t1.detach();
    //thread : recv
    std::thread t2(receiveProcess);
    t2.detach();

    //thread : handle rq
    std::thread t3(handleRqProcess);
    t3.detach();

    //thread : handle hq
    std::thread t4(handleHqProcess);
    t4.detach();
    while (keepRunning)
    {
        sleep(1);
    }
    cout << "Terminated by Ctrl+C signal." << endl;
    return 0;
}