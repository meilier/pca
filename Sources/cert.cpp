#include "cert.hpp"
#include <fstream>
#include <sstream>
#include <stdio.h>
using namespace std;


/* **********
 * Call openssl ca command to sign the crs file the client requests.
 */
void Cert::signCert(string certType){
    //call openssl command to sign 
    string signCmd = "openssl ca -in " + getCertFileName("csr",certType) + " -out " + getCertFileName("pem",certType) + "-batch -key meilier";
    //Todo: error handling
    popen(signCmd.c_str(),"w");
}

/* **********
 * get all certs at certs/ dir in a tar.gz file
 */
void Cert::getAllCerts(){
    //wait for an abstract
    string compactCmd = " tar -zcvf certs.tar.gz certs/";
    popen(compactCmd.c_str(),"w");
}

/* **********
 * revoke account and tls cert of one node
 */
void Cert::revokeCert(){

    //use ca private key get plaintext(tar.gz including node.pem),digest and node.signature
    //ca server use pem in plaintext to analysis the corresponding signature to make sure its deconding message equaling to digest

    //decoding message
    
    string invokeAccountCmd = "openssl ca -revoke "  + getCertFileName("pem","account") + " -key meilier";
    string invokeTlsCmd = "openssl ca -revoke "  + getCertFileName("pem","tls") + " -key meilier";
    string genAccountCrlCmd = "openssl ca -gencrl -out" + getCertFileName("crl","account");
    string genTlsCrlCmd = "openssl ca -gencrl -out" + getCertFileName("crl","tls");
    popen(invokeAccountCmd.c_str(),"w");
    popen(invokeTlsCmd.c_str(),"w");
    popen(genAccountCrlCmd.c_str(),"w");
    popen(genTlsCrlCmd.c_str(),"w");
}

/* **********
 * Call openssl ca command to sign the crs file the client requests.
 */
string Cert::getCertFileName(string fileType, string useType){
    stringstream ss;
    ss << aCertSerial;
    string acs = ss.str();
    if(fileType == "csr"){
        if(useType == "account"){
            return nodeAccountRequest + accountCert + ".csr";
        }else{
            return nodeTlsRequest + tlsCert + ".csr";
        }
    }else if(fileType == "pem"){
        if(useType == "account"){
            return nodeAccountCert + accountCert + ".pem";
        }else{
            return nodetlsCert + tlsCert + ".csr";
        }
    }else if(fileType == "crl"){
        if(useType == "account"){
            return nodeAccountCrl + accountCert + ".crl";
        }else{
            return nodeTlsCrl + tlsCert + ".crl";
        }
    }
}

