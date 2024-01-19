#!/bin/bash
source .env
SERVER_STRING=ftp://${FTP_USER}:${FTP_PASS}@${FTP_ADDR}/atmosphere/contents/4100000000000324/

pushd dist/4100000000000324
ftp -u ${SERVER_STRING} exefs.nsp toolbox.json
pushd flags
ftp -u ${SERVER_STRING}flags/ boot2.flag
popd
popd
