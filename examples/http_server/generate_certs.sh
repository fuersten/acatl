#/bin/sh
mkdir certs > /dev/null 2>&1
cd certs

openssl req -nodes -x509 -days 365 -newkey rsa:2048 -out server.pem -keyout key.pem -subj "/C=DE/ST=HH/L=Hamburg"

cd ..
