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
    inst_multiple hostname
    inst_hook pre-mount 09 "$moddir/minishell.sh"
    mkdir -p "${initdir}/etc/init.d"
    inst_script "$moddir/rcS.sh" "/etc/init.d/rcS"
    inst_simple "$moddir/inittab" "/etc/inittab"
}
