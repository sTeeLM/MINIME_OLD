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
    inst_script "$moddir/merge-overlay.sh" "/bin/merge-overlay"
    inst_hook pre-mount 08 "$moddir/merge-overlay-hook.sh"
    inst_dir /mnt 
}
