#!/bin/sh 
PATH=/sbin:/bin:/usr/sbin:/usr/bin 
runlevel=S 
prevlevel=N 
umask 022 
export PATH runlevel prevlevel
echo "3 1 4 3" > /proc/sys/kernel/printk
clear
hostname MINIME
echo 'export HOSTNAME=MINIME' > /etc/profile
echo 'export USER=root' >> /etc/profile
echo 'export PS1="[\u@\h \W]#"' >> /etc/profile
