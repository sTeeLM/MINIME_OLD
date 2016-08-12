#!/bin/bash
# -*- mode: shell-script; indent-tabs-mode: nil; sh-basic-offset: 4; -*-
# ex: ts=8 sw=4 sts=4 et filetype=sh

check() {
    # a live host-only image doesn't really make a lot of sense
    [[ $hostonly ]] && return 1
    return 255
}

depends() {
    # if dmsetup is not installed, then we cannot support fedora/red hat
    # style live images
    return 0
}

installkernel() {
    return 0
}

install() {
    local _i _progs _path _busybox
    inst_multiple hostname which clear swapon swapoff udevadm dwipe
    inst_hook pre-mount 09 "$moddir/minishell.sh"
    mkdir -p "${initdir}/etc/init.d"
    inst_script "$moddir/rcS.sh" "/etc/init.d/rcS"
    inst_simple "$moddir/inittab" "/etc/inittab"
    inst_simple "$moddir/profile.minishell" "/etc/profile.minishell"
    inst_simple "/etc/minime.splash" "/etc/minime.splash"

    _busybox=$(type -P busybox)
    inst $_busybox /usr/bin/busybox

    for _i in $($_busybox | sed -ne '1,/Currently/!{s/,//g; s/busybox//g; p}')
    do
        _progs="$_progs $_i"
    done

    # FIXME: switch_root should be in the above list, but busybox version hangs
    # (using busybox-1.15.1-7.fc14.i686 at the time of writing)

    for _i in $_progs; do
        inst_simple $_i
    done

    # do not use sh of busybox!
    inst /bin/bash && ln -sf bash "${initdir}/bin/sh" 

    # do not use systemd as init
    rm -rf "${initdir}/sbin/init"
    ln -sf ../bin/busybox "${initdir}/sbin/init" 

    # do not use systemd's poweroff and reboot
    rm -rf "${initdir}/sbin/poweroff"
    ln -sf ../bin/busybox "${initdir}/sbin/poweroff"
    rm -rf "${initdir}/sbin/reboot"
    ln -sf ../bin/busybox "${initdir}/sbin/reboot"
}
