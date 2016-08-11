#!/bin/sh 
PATH=/sbin:/bin:/usr/sbin:/usr/bin 
runlevel=S 
prevlevel=N 
umask 022 
export PATH runlevel prevlevel
echo "3 1 4 3" > /proc/sys/kernel/printk
clear
hostname MINIME
mv /etc/profile.minishell /etc/profile
