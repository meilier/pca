ScirptPath=$(cd `dirname $0`; pwd)
echo $ScirptPath
source $ScirptPath/header.sh
mkdir -p $CLIENTPATH
cp $WORKDIR/Scripts/openssl.cnf $CLIENTPATH/
mkdir -p $CLIENTPATH/certs/account $CLIENTPATH/certs/tls
mkdir -p $CLIENTPATH/requests/account $CLIENTPATH/requests/tls
mkdir -p $CLIENTPATH/crl
echo "set directory done"
openssl genrsa -aes256 -passout pass:$CLIENTPASS -out $CLIENTPATH/certs/account/account.key.pem 2048
chmod 400 $CLIENTPATH/certs/account/account.key.pem
openssl genrsa -aes256 -passout pass:$CLIENTPASS -out $CLIENTPATH/certs/tls/tls.key.pem 2048
chmod 400 $CLIENTPATH/certs/tls/tls.key.pem
echo "generate client account and tls key done"

openssl req -config $CLIENTPATH/openssl.cnf -new -key $CLIENTPATH/certs/account/account.key.pem -out $CLIENTPATH/requests/account/account.csr -passin pass:$CLIENTPASS -subj /C=$COUNTRYNAME/ST=$STATEPROVINCENAME/O=$ORGNAME/OU=$ORGUNITNAME/CN=$COMMONNAMEACCOUNT
openssl req -config $CLIENTPATH/openssl.cnf -new -key $CLIENTPATH/certs/tls/tls.key.pem -out $CLIENTPATH/requests/tls/tls.csr -passin pass:$CLIENTPASS -subj /C=$COUNTRYNAME/ST=$STATEPROVINCENAME/O=$ORGNAME/OU=$ORGUNITNAME/CN=$COMMONNAMETLS
echo "generate client csr files done"