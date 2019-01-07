ScirptPath=$(cd `dirname $0`; pwd)
echo $ScirptPath
source $ScirptPath/header.sh
mkdir -p $CLIENTPATH
cp $WORKDIR/Scripts/openssl.cnf $CLIENTPATH/
mkdir -p $CLIENTPATH/certs/account $CLIENTPATH/certs/tls
mkdir -p $CLIENTPATH/requests/account $CLIENTPATH/requests/tls
mkdir -p $CLIENTPATH/crl
echo "set directory done"
openssl ecparam -name secp384r1 -genkey | openssl ec -aes-256-cbc -passout pass:"123456" -out $CLIENTPATH/certs/account/account.key.pem
chmod 400 $CLIENTPATH/certs/account/account.key.pem
openssl ecparam -name secp384r1 -genkey | openssl ec -aes-256-cbc -passout pass:"123456" -out $CLIENTPATH/certs/tls/tls.key.pem
chmod 400 $CLIENTPATH/certs/tls/tls.key.pem
echo "generate client account and tls key done"

openssl req -config $CLIENTPATH/openssl.cnf -new -key $CLIENTPATH/certs/account/account.key.pem -out $CLIENTPATH/requests/account/account.csr -passin pass:"123456" -subj '/C=US/ST=California/O=XXXX/OU=XXXX Test/CN=account Test 1'
openssl req -config $CLIENTPATH/openssl.cnf -new -key $CLIENTPATH/certs/tls/tls.key.pem -out $CLIENTPATH/requests/tls/tls.csr -passin pass:"123456" -subj '/C=US/ST=California/O=XXXX/OU=XXXX Test/CN=tls Test 1'
echo "generate client csr files done"