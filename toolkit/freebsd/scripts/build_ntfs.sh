#!/bin/sh
TEMP_DIR=temp

mkdir ${TEMP_DIR}
cd ${TEMP_DIR}

mkdir -p sbin
mkdir -p libexec
mkdir -p lib
mkdir -p usr/local/bin
mkdir -p usr/local/sbin
mkdir -p usr/local/lib

cp  /sbin/mount_fusefs sbin/mount_fusefs
cp  /libexec/ld-elf.so.1 libexec/ld-elf.so.1
cp  /usr/local/bin/ntfsfix usr/local/bin/ntfsfix
cp  /usr/local/bin/ntfsinfo usr/local/bin/ntfsinfo
cp  /usr/local/bin/ntfscluster usr/local/bin/ntfscluster
cp  /usr/local/bin/ntfsls usr/local/bin/ntfsls
cp  /usr/local/bin/ntfscat usr/local/bin/ntfscat
cp  /usr/local/bin/ntfscmp usr/local/bin/ntfscmp
cp  /usr/local/bin/ntfs-3g.probe usr/local/bin/ntfs-3g.probe
cp  /usr/local/bin/ntfs-3g.usermap usr/local/bin/ntfs-3g.usermap
cp  /usr/local/bin/ntfs-3g.secaudit usr/local/bin/ntfs-3g.secaudit
cp  /usr/local/bin/ntfs-3g usr/local/bin/ntfs-3g
cp  /usr/local/bin/lowntfs-3g usr/local/bin/lowntfs-3g

# libs
cp  /usr/local/lib/libfuse.so.2 usr/local/lib/libfuse.so.2
cp  /usr/local/lib/libntfs-3g.so.86 usr/local/lib/libntfs-3g.so.86
cp  /usr/local/lib/libublio.so.1 usr/local/lib/libublio.so.1
cp  /lib/libc.so.7 lib/libc.so.7
cp  /lib/libthr.so.3 lib/libthr.so.3
cp  /usr/local/sbin/mkntfs usr/local/sbin/mkntfs
cp  /usr/local/sbin/ntfslabel usr/local/sbin/ntfslabel
cp  /usr/local/sbin/ntfsundelete usr/local/sbin/ntfsundelete
cp  /usr/local/sbin/ntfsresize usr/local/sbin/ntfsresize
cp  /usr/local/sbin/ntfsclone usr/local/sbin/ntfsclone
cp  /usr/local/sbin/ntfscp usr/local/sbin/ntfscp

tar -cf - * | xz -c > ../ntfs3g.`uname -m`.txz

cd ..

rm -rf ${TEMP_DIR}
