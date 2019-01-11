#include "cert.hpp"
#include "common.hpp"
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <stdio.h>
using namespace std;

/* **********
 * Call openssl ca command to sign the crs file the client requests.
 */
void Cert::signCert(int conn, string certType)
{
    //call openssl command to sign
    printf("start to sign cert\n");
    string signCmd = "openssl ca -config " + configPath + " -in " + getCertFileName(conn, "csr", certType) + " -out " + getCertFileName(conn, "pem", certType) + " -batch -key 123456";
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
void Cert::revokeCert(int conn)
{

    //use ca private key get plaintext(tar.gz including node.pem),digest and node.signature
    //ca server use pem in plaintext to analysis the corresponding signature to make sure its deconding message equaling to digest

    //decoding message

    string invokeAccountCmd = "openssl ca -config " + configPath + " -revoke " + getCertFileName(conn, "pem", "account") + " -key meilier";
    string invokeTlsCmd = "openssl ca -config " + configPath + " -revoke " + getCertFileName(conn, "pem", "tls") + " -key meilier";
    string genCrlCmd = "openssl ca -config " + configPath + " -gencrl -out" + getCertFileName(conn, "crl");
    popen(invokeAccountCmd.c_str(), "w");
    popen(invokeTlsCmd.c_str(), "w");
    popen(genCrlCmd.c_str(), "w");
}

/* **********
 * Call openssl ca command to sign the crs file the client requests.
 */
string Cert::getCertFileName(int conn, string fileType, string useType)
{
    string returnmsg;
    if (fileType == "csr")
    {
        if (useType == "account")
        {

            returnmsg = nodeAccountRequest + accountCert + to_string(CertSerial.find(conn)->second) + ".csr";
        }
        else
        {
            returnmsg = nodeTlsRequest + tlsCert + to_string(CertSerial.find(conn)->second) + ".csr";
        }
    }
    else if (fileType == "pem")
    {
        if (useType == "account")
        {
            returnmsg = nodeAccountCert + accountCert + to_string(CertSerial.find(conn)->second) + ".pem";
        }
        else
        {
            returnmsg = nodetlsCert + tlsCert + to_string(CertSerial.find(conn)->second) + ".pem";
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
    serial++;
}

int Cert::getSerial()
{
    return serial;
}

void Cert::insertSerial(int conn, int serial)
{
    CertSerial.insert(pair<int, int>(conn, serial));
}

void Cert::deleteSerial(int conn)
{
    CertSerial.erase(conn);
}

Cert::Cert()
{
    printf("Init CA Server\n");
    //call setup.sh
    char current_absolute_path[512];
    getcwd(current_absolute_path, 512);
    //int index = strrchr( current_absolute_path, '/' ) - current_absolute_path;
    //current_absolute_path[index] = '\0';
    WORKDIR = string(current_absolute_path);
    printf("WORKDIR is %s \n", WORKDIR.c_str());
    //readConfigFile((WORKDIR+"/Config/config.cfg").c_str(),"CAPATH",CAPATH);
    //printf("CAPATH is %s\n",CAPATH.c_str());
    //printf("nodeCert is %s\n",nodeCert.c_str());
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