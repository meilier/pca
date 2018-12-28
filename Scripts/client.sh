export CAPATH = "/Users/xingweizheng/client"
mkdir CAPATH
cp openssl.cnf $CAPATH/
mkdir -p $CAPATH/certs/account $CAPATH/certs/tls
mkdir -p $CAPATH/requests/account $CAPATH/requests/tls
mkdir -p $CAPATH/crl
openssl ecparam -name secp384r1 -genkey | openssl ec -aes-256-cbc -passout pass:"123456" -out $CAPATH/certs/account/testaccount.key.pem
chmod 400 $CAPATH/certs/account/testaccount.key.pem
openssl ecparam -name secp384r1 -genkey | openssl ec -aes-256-cbc -passout pass:"123456" -out $CAPATH/certs/account/testtls.key.pem
chmod 400 $CAPATH/certs/tls/testtls.key.pem

openssl req -config $CAPATH/openssl.cnf -new -key $CAPATH/certs/account/testaccount.key.pem -out $CAPATH/requests/account/test1.csr.pem -passin pass:"123456" -subj '/C=US/ST=California/O=XXXX/OU=XXXX Test/CN=XXXX Test 1'
openssl req -config $CAPATH/openssl.cnf -new -key $CAPATH/certs/account/testaccount.key.pem -out $CAPATH/requests/account/test1.csr.pem -passin pass:"123456" -subj '/C=US/ST=California/O=XXXX/OU=XXXX Test/CN=XXXX Test 1'



