# setup dir
ScirptPath=$(cd `dirname $0`; pwd)
echo $ScirptPath
source $ScirptPath/header.sh
mkdir -p $CAPATH
cp $WORKDIR/Scripts/openssl.cnf $CAPATH/
mkdir -p $CAPATH/certs/account $CAPATH/certs/tls
mkdir -p $CAPATH/requests/account $CAPATH/requests/tls
mkdir -p $CAPATH/crl $CAPATH/private $CAPATH/newcerts
touch $CAPATH/index.txt
echo '00' > $CAPATH/serial
echo "set up directoty ok" 

# create ca root private key and pem
openssl ecparam -name secp384r1 -genkey | openssl ec -aes-256-cbc -passout pass:"123456" -out $CAPATH/private/root.key.pem
chmod 400 $CAPATH/private/root.key.pem
openssl req -config $CAPATH/openssl.cnf -key $CAPATH/private/root.key.pem -new -extensions ext_root -out $CAPATH/private/root.cert.pem -x509 -passin pass:"123456" -subj '/C=US/ST=California/O=XXXX/OU=XXXX Certificate Authority/CN=XXXX Root CA' -days 7300
chmod 444 $CAPATH/private/root.cert.pem
echo "generate ca file ok"

# done 
echo "done everything"
exit
