#!/bin/sh

dwipe_find_dev()
{
    local _dev="$1"
    _dev=${_dev##*/}
    _dev=/sys/class/block/$_dev
    _dev=$(readlink $_dev)
    _dev=${_dev##*/block/}
    _dev=${_dev%%/*}
    echo /dev/$_dev
}

_dwipe_shell()
{
    local _name="$1"
    local _cmd="$2"
    debug_off
    export PS1="$_name:\${PWD}# "
    [ -e /.profile ] || >/.profile

    mkdir -p /var/log/dban

    echo "export TERM=linux" > /etc/profile
    echo "$_cmd" >> /etc/profile
    echo 'echo Press ENTER to reboot' >> /etc/profile
    echo 'read' >> /etc/profile
    echo "reboot" >> /etc/profile

    _ctty="$(RD_DEBUG= getarg rd.ctty=)" && _ctty="/dev/${_ctty##*/}"
    if [ -z "$_ctty" ]; then
        _ctty=console
        while [ -f /sys/class/tty/$_ctty/active ]; do
            _ctty=$(cat /sys/class/tty/$_ctty/active)
    	    _ctty=${_ctty##* } # last one in the list
        done
        _ctty=/dev/$_ctty
    fi
    [ -c "$_ctty" ] || _ctty=/dev/tty1
    case "$(/usr/bin/setsid --help 2>&1)" in *--ctty*) CTTY="--ctty";; esac
    setsid $CTTY /bin/sh -i -l 0<>$_ctty 1<>$_ctty 2<>$_ctty
}

dwipe_shell()
{
    local _ctty
    local _cmd="$1"
    set +e
    local _rdshell_name="dracut" action="Boot" hook="emergency"
    if [ "$1" = "-n" ]; then
        _rdshell_name=$2
        shift 2
    elif [ "$1" = "--shutdown" ]; then
        _rdshell_name=$2; action="Shutdown"; hook="shutdown-emergency"
        if type plymouth >/dev/null 2>&1; then
            plymouth --hide-splash
        elif [ -x /oldroot/bin/plymouth ]; then
            /oldroot/bin/plymouth --hide-splash
        fi
        shift 2
    fi

    echo ; echo
    warn "$*"
    source_hook "$hook"
    echo

    _dwipe_shell "$_rdshell_name" "$_cmd"
  
    reboot
}

if getargbool 0 rd.dwipe ; then
    if getargbool 0 rd.dwipe.auto ; then
        dwipe_cmd="dwipe -a"
    else
        dwipe_cmd="dwipe"
    fi

    dwipe_exclude=$(getarg rd.dwipe.exclude=)
    dwipe_exclude=$(devnames $dwipe_exclude)
    dwipe_exclude=$(dwipe_find_dev $dwipe_exclude)

    info "dwipe_cmd is $dwipe_cmd, dwipe_auto is $dwipe_auto, dwipe_exlude is $dwipe_exlude"

    dwipe_shell "$dwipe_cmd -e $dwipe_exclude"
fi
