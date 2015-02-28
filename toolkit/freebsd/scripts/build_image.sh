#!/bin/sh

set -x

BOOT_IMG=$1
ISO_PATH=$2

ISO_NAME=`basename ${ISO_PATH}`
WORK_PATH=""
ISO_DEV=""
ISO_DIR=""
IMAGE_DEV=""
IMAGE_DIR=""
ROOTFS_IMG=""
ROOTFS_DEV=""
ROOTFS_DIR=""

ROOT_DIR=`pwd`

if [ -f "${BOOT_IMG}" ];
then
    echo "${BOOT_IMG} exist, will not overwrite it, quit!"
    exit 1
fi

if [ ! -f "${ISO_PATH}" ];
then
    echo "${ISO_PATH} not exist, quit!"
    exit 1
fi

# make work space

mkdir -p ${ROOT_DIR}/work
WORK_PATH=`pwd`/work


# mount src iso file

ISO_DIR=${WORK_PATH}/iso
mkdir -p ${ISO_DIR}
ISO_DEV=`mdconfig -a -t vnode -f ${ISO_PATH}`
mount_cd9660 /dev/${ISO_DEV} ${ISO_DIR}

# test media


# make boot.img as 100M
dd if=/dev/zero of=${BOOT_IMG} bs=1k count=100000

# Attach this image file as a memory disk:
IMAGE_DEV=`mdconfig -a -t vnode -f ${BOOT_IMG}`

# Write a BSD label with boot code to it:
bsdlabel -w -B ${IMAGE_DEV} auto

# And create a filesystem on it (BSD uses ufs):
newfs -m 0 "${IMAGE_DEV}a"

# Make image dir
IMAGE_DIR=${WORK_PATH}/img
mkdir -p ${IMAGE_DIR}
mount /dev/${IMAGE_DEV}a ${IMAGE_DIR}

# retrive boot
cat ${WORK_PATH}/iso/usr/freebsd-dist/kernel.txz | xz -d | tar -C ${IMAGE_DIR} \
   --include=boot/kernel/unionfs.ko \
   --include=boot/kernel/cd9660.ko \
   --include=boot/kernel/msdosfs.ko \
   --include=boot/kernel/nullfs.ko \
   --include=boot/kernel/kernel \
   -xvf - 

cat ${WORK_PATH}/iso/usr/freebsd-dist/base.txz | xz -d | tar -C ${IMAGE_DIR} \
   --include=boot \
   -xvf - 

# copy /boot/loader.conf
cp ${ROOT_DIR}/scripts/loader.conf ${IMAGE_DIR}/boot


# create ROOTFS
ROOTFS_IMG=${IMAGE_DIR}/data/rootfs.img
mkdir -p ${IMAGE_DIR}/data

# make rootfs.img as 30M
dd if=/dev/zero of=${ROOTFS_IMG} bs=1k count=30000
ROOTFS_DEV=`mdconfig -a -t vnode -f ${ROOTFS_IMG}`

# Write a BSD label with boot code to it:
bsdlabel -w -B ${ROOTFS_DEV} auto

# And create a filesystem on it (BSD uses ufs):
newfs -m 0 "${ROOTFS_DEV}a"

ROOTFS_DIR=${WORK_PATH}/rootfs
mkdir -p ${ROOTFS_DIR}
mount /dev/${ROOTFS_DEV}a ${ROOTFS_DIR}

# retrive rescue
cat ${WORK_PATH}/iso/usr/freebsd-dist/base.txz | xz -d | tar -C ${ROOTFS_DIR} \
   --include=rescue \
   -xvf - 

# copy baseroot.rc
cp ${ROOT_DIR}/scripts/baseroot.rc ${ROOTFS_DIR}
cp -r ${ROOT_DIR}/scripts ${ROOTFS_DIR}
mkdir -p ${ROOTFS_DIR}/dev

# create iso.rc
echo BSD_ISO=\"${ISO_NAME}\" > ${ROOTFS_DIR}/iso.rc

################
umount ${ROOTFS_DIR}
mdconfig -d -u `echo ${ROOTFS_DEV} | cut -c 3-`

umount ${IMAGE_DIR}
mdconfig -d -u `echo ${IMAGE_DEV} | cut -c 3-`

umount ${ISO_DIR}
mdconfig -d -u `echo ${ISO_DEV} | cut -c 3-`

rm -rf ${WORK_PATH}
