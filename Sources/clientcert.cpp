#include "client.hpp"
#include "common.hpp"

string ClientCert::getCertFileName(string fileType, string certType)
{
    if (fileType == "csr")
    {
        if (certType == "account")
        {

            return nodeAccountRequest + accountCert + ".csr";
        }
        else
        {
            return nodeTlsRequest + tlsCert + ".csr";
        }
    }
    else if (fileType == "pem")
    {
        if (certType == "account")
        {
            return nodeAccountCert + accountCert + ".pem";
        }
        else
        {
            return nodetlsCert + tlsCert + ".pem";
        }
    }
    else if (fileType == "crl")
    {
        //crl file contains all certs that have been invoked
        return nodeCrl + "invoke.crl";
    }
    else if (fileType == "compact")
    {
        return CLIENTPATH + "certs.tar.gz";
    }
    return "error: incorrect fileType";
}

ClientCert::ClientCert()
{
    printf("Init CA Server\n");
    //call setup.sh
    string signCmd = "sh " + string(WORKDIR) + "/Scripts/client.sh";
    //Todo: error handling
    popen(signCmd.c_str(), "w");
}
ClientCert::~ClientCert()
{
    printf("Clear CA Server\n");
    //call setup.sh
    string signCmd = "sh " + string(WORKDIR) + "/Scripts/clearclient.sh";
    //Todo: error handling
    popen(signCmd.c_str(), "w");
}