#include "common.hpp"



class Cert{
    private:
        int aCertSerial = 0;
        int tCertSerial = 0;
        //root path
        const string CAPATH = "/Users/weizhengxing/cafolder/";
        const string caCert = CAPATH;
        //first-tier path
        const string nodeCert = CAPATH + "certs";
        const string nodeRequest = CAPATH + "requests";
        const string nodeCrl = CAPATH + "crl";

        //sencode-tier path
        const string nodeAccountCert = nodeCert + "account";
        const string nodetlsCert = nodeCert + "tls";
        const string nodeAccountRequest = nodeRequest + "account";
        const string nodeTlsRequest = nodeRequest + "tls";
        //const string nodeAccountCrl = nodeCrl + "account";
        //const string nodeTlsCrl = nodeCrl + "tls";

        //filename
        const string accountCert = "accountCert";
        const string tlsCert = "tlsCert";

    public:
        void signCert(string certType);
        void getAllCerts();
        void revokeCert();

        string getCertFileName(string fileType, string useType = "");
};