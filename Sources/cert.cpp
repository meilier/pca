#include "cert.hpp"
#include "common.hpp"
#include <fstream>
#include <sstream>
#include <stdio.h>
using namespace std;

/* **********
 * Call openssl ca command to sign the crs file the client requests.
 */
void Cert::signCert(string certType)
{
    //call openssl command to sign
    printf("start to sign cert\n");
    string signCmd = "openssl ca -config " + configPath + " -in " + getCertFileName("csr", certType) + " -out " + getCertFileName("pem", certType) + " -batch -key 123456";
    printf("this command is %s\n", signCmd.c_str());
    //Todo: error handling
    popen(signCmd.c_str(), "w");
    printf("sign cert ok\n");
}

/* **********
 * get all certs at certs/ dir in a tar.gz file
 */
void Cert::getAllCerts()
{
    //wait for an abstract
    string compactCmd = " tar -zcvf " + CAPATH + "/certs.tar.gz" + " -C " + CAPATH + " certs ";
    popen(compactCmd.c_str(), "w");
}

/* **********
 * revoke account and tls cert of one node
 */
void Cert::revokeCert()
{

    //use ca private key get plaintext(tar.gz including node.pem),digest and node.signature
    //ca server use pem in plaintext to analysis the corresponding signature to make sure its deconding message equaling to digest

    //decoding message

    string invokeAccountCmd = "openssl ca -config " + configPath + " -revoke " + getCertFileName("pem", "account") + " -key meilier";
    string invokeTlsCmd = "openssl ca -config " + configPath + " -revoke " + getCertFileName("pem", "tls") + " -key meilier";
    string genCrlCmd = "openssl ca -config " + configPath + " -gencrl -out" + getCertFileName("crl");
    popen(invokeAccountCmd.c_str(), "w");
    popen(invokeTlsCmd.c_str(), "w");
    popen(genCrlCmd.c_str(), "w");
}

/* **********
 * Call openssl ca command to sign the crs file the client requests.
 */
string Cert::getCertFileName(string fileType, string useType)
{
    string returnmsg;
    if (fileType == "csr")
    {
        if (useType == "account")
        {

            returnmsg = nodeAccountRequest + accountCert + to_string(CertSerial) + ".csr";
        }
        else
        {
            returnmsg = nodeTlsRequest + tlsCert + to_string(CertSerial) + ".csr";
        }
    }
    else if (fileType == "pem")
    {
        if (useType == "account")
        {
            returnmsg = nodeAccountCert + accountCert + to_string(CertSerial) + ".pem";
        }
        else
        {
            returnmsg = nodetlsCert + tlsCert + to_string(CertSerial) + ".pem";
        }
    }
    else if (fileType == "crl")
    {
        //crl file contains all certs that have been invoked
        returnmsg = nodeCrl + "invoke.crl";
    }
    else if (fileType == "compact")
    {
        returnmsg = CAPATH + "certs.tar.gz";
    }
    else
    {
        returnmsg = "error";
    }
    printf("Cert::getCertFileName : returnmsg is %s\n", returnmsg.c_str());
    return returnmsg;
}

void Cert::increaseSerial()
{
    CertSerial++;
}

Cert::Cert()
{
    printf("Init CA Server\n");
    //call setup.sh
    string signCmd = "sh " + string(WORKDIR) + "/Scripts/setup.sh";
    //Todo: error handling
    popen(signCmd.c_str(), "w");
}
Cert::~Cert()
{
    printf("Clear CA Server\n");
    //call setup.sh
    string signCmd = "sh " + string(WORKDIR) + "/Scripts/clear.sh";
    //Todo: error handling
    //popen(signCmd.c_str(), "w");
}