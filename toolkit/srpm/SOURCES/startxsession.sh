#!/bin/sh
export PATH=$PATH:/usr/dt/bin
export LANG=zh_CN.UTF-8
systemctl start rpcbind.service
/usr/dt/bin/Xsession
