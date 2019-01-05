# bug

## change thread loop to semaphore



## bug1

Cert::getCertFileName : returnmsg is /Users/xingweizheng/testecc/requests/account/accountCert1.csrfileProcess: why thead not return 538

//here not continue to execute
fileProcess: why thead not return 0
should be ready to return
start to sign cert

            csrfile.close();
            //send file get ok message to handle process
            certType == 0 ? hq.Push(GACO) : hq.Push(GTCO);
            printf("should be ready to return\n");
            return;
        309 not put message to hq

### bug 1 --solusion

    You need to close socket first, otherwise the server will still wait data coming.

    244 printf("sendProcess: send csr successfully\n");
        //may be the bug is you need to close socket first
        close(file_cli);
        sfile.close();
        //send file get ok message to handle process
        //may be h

## bug 2

Write out database with 1 new entries
Data Base Updated
Cert::getCertFileName : returnmsg is /Users/xingweizheng/testecc/certs/account/accountCert1.pem

//here the server stop to run  --1⃣️
reveiceProcess: the message is
start message listening thread at 7000 // here prove that sema works because of the absence of client
wrong message
reveiceProcess: the message is
wrong message
reveiceProcess: the message is
wrong message

next, I'll see why sq.Push(SAO) don't work.
            if (hqmessage == GACO)
            {
                //sign account certificate
                mCert->signCert("account");
                sq.Push(SAO);
        249 }


