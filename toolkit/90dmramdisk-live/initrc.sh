#!/bin/sh
export PS1="MINIME:\${PWD}# "
export TERM=linux
export PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin

_ctty=console
while [ -f /sys/class/tty/$_ctty/active ]; do
    _ctty=$(cat /sys/class/tty/$_ctty/active)
    _ctty=${_ctty##* } # last one in the list
done
_ctty=/dev/$_ctty
[ -c "$_ctty" ] || _ctty=/dev/tty1
setsid -c -w /bin/bash -i -l 0<>$_ctty 1<>$_ctty 2<>$_ctty
/shutdown 
