#include "cert.hpp"
#include <stdlib.h>

#define WORKDIR "/Users/xingweizheng/github/pca"
int main()
{
    FILE *fp;
    char buf[1024];
    //call setup.sh
    string signCmd = " openssl req -in /Users/xingweizheng/testrsa/requests/account/accountCert1.csr -noout -text |grep ST| awk  '{print $6}'|awk -F= '{print $2}'";
    //Todo: error handling
    printf("hello world\n");
    if ((fp = popen(signCmd.c_str(), "w")) == NULL)
    {
        printf("failed to popen");
    }else{
        printf("It's OK!\n");
    }
    fp == NULL ? printf("yes\n"):printf("no\n");
    while (fgets(buf, 200, fp) != NULL)
    {
        printf("%s\n", buf);
    }
    pclose(fp);
    return 0;
}