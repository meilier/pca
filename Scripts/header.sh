WORKDIR="/Users/xingweizheng/github/pca"
CONFIG=$WORKDIR/Config/config.cfg

# if config file enable?
source $CONFIG
echo "enable config ?" $ENABLECONFIG
if [ $ENABLECONFIG -eq 0 ]
then
# set var to default
CAPATH="/Users/xingweizheng/testecc"
CLIENTPATH="/Users/xingweizheng/client"
CAPASS=123456
CLIENTPASS=123456
COUNTRYNAME=CN
STATEPROVINCENAME=ZHEJIANG
ORGNAME=ZJU
ORGUNITNAME=ZJUCS
COMMONNAMECA=CA
COMMONNAMEACCOUNT=ACCOUNT
COMMONNAMETLS=TLS
fi
