# PCA

## CA Layer

**Name Rule**
cakey.pem
cacert.pem

account1.csr
account1.pem

tls1.csr
tls1.pem

.
├── certs
│   ├── account
│   │   └── some_server.pem
│   └── tls
│       └── node1tlscert.pem
├── crl
│   └── crl.pem
├── crlnumber
├── crlnumber.old
├── index.txt
├── index.txt.attr
├── index.txt.attr.old
├── index.txt.old
├── newcerts
│   ├── 1234.pem
│   └── 1235.pem
├── private
│   ├── cacert.pem
│   └── cakey.pem
├── requests
│   ├── account
│   │   └── some_server.csr
│   └── tls
│       └── node1tls.csr
├── serial
└── serial.old

## Client Layer

**Name Rule**
account.key.pem
account.csr
account.pem

tls.key.pem
tls.csr
tls.pem

.
├── certs
│   ├── account
│   │   └── testaccount.key.pem
│   └── tls
│       └── testtls.key.pem
├── crl
├── openssl.cnf
└── requests
    ├── account
    │   └── test1.csr.pem
    └── tls
        └── test1.csr.pem