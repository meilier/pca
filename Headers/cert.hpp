#include "common.hpp"

int exitTime = 0;

class Cert
{
  private:
    Cert();
    //Cert(Cert const &);
    //void operator=(Cert const &);
    ~Cert();
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
    Cert(Cert const &) = delete;
    void operator=(Cert const &) = delete;
    static Cert &getInstance()
    {
        static Cert theSingleton;
        return theSingleton;
    }

    void signCert(string certType);
    void getAllCerts();
    void revokeCert();

    string getCertFileName(string fileType, string useType = "");
};

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
    printf("exitTime is %d\n", exitTime);
    printf("Clear CA Server\n");
    //call setup.sh
    string signCmd = "sh " + string(WORKDIR) + "/Scripts/clear.sh";
    //Todo: error handling
    popen(signCmd.c_str(), "w");
    exitTime++;
}