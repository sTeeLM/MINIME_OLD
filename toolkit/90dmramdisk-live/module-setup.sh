#!/bin/bash
# -*- mode: shell-script; indent-tabs-mode: nil; sh-basic-offset: 4; -*-
# ex: ts=8 sw=4 sts=4 et filetype=sh

# called by dracut
check() {
    # a live host-only image doesn't really make a lot of sense
    [[ $hostonly ]] && return 1
    return 255
}

# called by dracut
depends() {
    # if dmsetup is not installed, then we cannot support fedora/red hat
    # style live images
    echo dm rootfs-block
    return 0
}

# called by dracut
installkernel() {
    instmods squashfs loop iso9660 rd
}

# called by dracut
install() {
    inst_multiple umount dmsetup blkid dd losetup grep blockdev e2label md5sum tar mkfs.ext4
    inst_hook pre-udev 30 "$moddir/dmmemdisk-live-genrules.sh"
    inst_script "$moddir/dmmemdisk-live-root.sh" "/sbin/dmmemdisk-live-root"
    inst_script "$moddir/initrc.sh" "/sbin/initrc"
    # should probably just be generally included
    dracut_need_initqueue
}
