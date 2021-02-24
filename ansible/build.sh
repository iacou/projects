cp ~/.ssh/id_rsa.pub .
docker build . -t debian-ed
rm id_rsa.pub
