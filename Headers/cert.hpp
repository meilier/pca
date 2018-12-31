#include "common.hpp"
class Cert
{
  private:
    Cert();
    //Cert(Cert const &);
    //void operator=(Cert const &);
    ~Cert();
    int CertSerial = 0;
    //root path
    const string CAPATH = "/Users/weizhengxing/testecc/";
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
    void increaseSerial();
    void signCert(string certType);
    void getAllCerts();
    void revokeCert();

    string getCertFileName(string fileType, string useType = "");
};

