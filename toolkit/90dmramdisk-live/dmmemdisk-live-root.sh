#!/bin/sh
# -*- mode: shell-script; indent-tabs-mode: nil; sh-basic-offset: 4; -*-
# ex: ts=8 sw=4 sts=4 et filetype=sh

type getarg >/dev/null 2>&1 || . /lib/dracut-lib.sh

PATH=/usr/sbin:/usr/bin:/sbin:/bin

modprobe rd rd_nr=1 rd_size=$((256*1024))

mkfs.ext4 /dev/ram0

mount /dev/ram0 /sysroot
pushd .
cd /
tar -cf - * --exclude=proc --exclude=run --exclude=sysroot --exclude=sys | tar -xf - -C /sysroot
mkdir /sysroot/proc
mkdir /sysroot/run
mkdir /sysroot/sys
rm /sysroot/sbin/init
rm /sysroot/init
mv /sysroot/sbin/initrc /sysroot/.bashrc
cat > /sysroot/etc/profile <<EOF
alias egrep='egrep --color=auto'
alias fgrep='fgrep --color=auto'
alias grep='grep --color=auto'
alias l.='ls -d .* --color=auto'
alias ll='ls -l --color=auto'
alias ls='ls --color=auto'
alias which='alias | /usr/bin/which --tty-only --read-alias --show-dot --show-tilde'
EOF
umount /sysroot
popd

ln -s /dev/ram0 /dev/root
printf 'mount %s /dev/ram0 %s\n' "$ROOTFLAGS" "$NEWROOT" > $hookdir/mount/01-$$-live.sh

need_shutdown

exit 0
