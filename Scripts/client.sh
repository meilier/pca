ScirptPath=$(cd `dirname $0`; pwd)
echo $ScirptPath
source $ScirptPath/header.sh
mkdir -p $CLIENTPATH
cp $WORKDIR/Scripts/openssl.cnf $CLIENTPATH/
mkdir -p $CLIENTPATH/certs/account $CLIENTPATH/certs/tls
mkdir -p $CLIENTPATH/requests/account $CLIENTPATH/requests/tls
mkdir -p $CLIENTPATH/crl
echo "set directory done"
openssl ecparam -name secp384r1 -genkey | openssl ec -aes-256-cbc -passout pass:"123456" -out $CLIENTPATH/certs/account/testaccount.key.pem
chmod 400 $CLIENTPATH/certs/account/testaccount.key.pem
openssl ecparam -name secp384r1 -genkey | openssl ec -aes-256-cbc -passout pass:"123456" -out $CLIENTPATH/certs/tls/testtls.key.pem
chmod 400 $CLIENTPATH/certs/tls/testtls.key.pem
echo "generate client account and tls key done"

openssl req -config $CLIENTPATH/openssl.cnf -new -key $CLIENTPATH/certs/account/testaccount.key.pem -out $CLIENTPATH/requests/account/test1.csr.pem -passin pass:"123456" -subj '/C=US/ST=California/O=XXXX/OU=XXXX Test/CN=XXXX Test 1'
openssl req -config $CLIENTPATH/openssl.cnf -new -key $CLIENTPATH/certs/tls/testtls.key.pem -out $CLIENTPATH/requests/tls/test1.csr.pem -passin pass:"123456" -subj '/C=US/ST=California/O=XXXX/OU=XXXX Test/CN=XXXX Test 1'
echo "generate client csr files done"