#!/bin/bash

# called by dracut
check() {
    [[ "$mount_needs" ]] && return 1
    require_binaries plymouthd plymouth plymouth-set-default-theme
}

# called by dracut
depends() {
    echo drm
}

# called by dracut
install() {
    if [ `uname -m` = "i686" ] ; then
        inst_multiple /usr/lib/plymouth/label.so
    else
        inst_multiple /usr/lib64/plymouth/label.so
    fi
#    inst /usr/share/fonts/minime/msyh.ttf /usr/share/fonts/minime/msyh.ttf
    inst /usr/share/X11/fonts/Type1/c0419bt_.pfb /usr/share/fonts/c0419bt_.pfb

    inst /var/lib/plymouth/boot-duration /var/lib/plymouth/boot-duration
}

