#!/bin/sh
# /dev/mapper/live-rw is overlayd rootdev

echo '---------------------------------------------------'
echo 'merge overlay mode by sTeeL<steel.mental@gmail.com>'
echo '---------------------------------------------------'
echo

type getarg >/dev/null 2>&1 || . /lib/dracut-lib.sh

live_dir=$(getarg rd.live.dir -d live_dir)
[ -z "$live_dir" ] && live_dir="LiveOS"
squash_image=$(getarg rd.live.squashimg)
[ -z "$squash_image" ] && squash_image="squashfs.img"

echo live_dir is ${live_dir}
echo squash_image is ${squash_image}

ROOT_DEV=/dev/mapper/live-rw

export LANG=C

declare -a options
declare -a block_device_name
declare -a block_device_type
declare -a block_device_total_size
declare -a block_device_free_size
declare -a block_device_label

root_dev_size=$(blockdev --getsize64 $ROOT_DEV)
min_temp_size=$(($root_dev_size * 2))
live_root_dir=/run/initramfs/live

if [ ! -e $live_root_dir/${live_dir}/${squash_image} -o ! -w $live_root_dir/${live_dir}/${squash_image} ]; then
    echo "$live_root_dir not exist or not writable, quit!"
    exit 1
fi

if [ ! -r $ROOT_DEV ]; then
    echo "$ROOT_DEV not readable, quit!"
    exit 1
fi

blockDevGetUsed()
{
    if [ -z "$(grep $1 /proc/mounts)" ]; then
        mkdir /mnt/merge_probe || :
        mount -n $1 /mnt/merge_probe || :
        echo -n $(df -B1 --output=used $1 | grep -v Used) || :
        umount /mnt/merge_probe || :
        rmdir /mnt/merge_probe
    else
        echo -n $(df -B1 --output=avail $1 | grep -v Avail)
    fi
}


blockDevGetFree()
{
    if [ -z "$(grep $1 /proc/mounts)" ]; then
        mkdir /mnt/merge_probe || :
        mount -n $1 /mnt/merge_probe || :
        echo -n $(df -B1 --output=avail $1 | grep -v Avail) || :
        umount /mnt/merge_probe || :
        rmdir /mnt/merge_probe
    else
        echo -n $(df -B1 --output=avail $1 | grep -v Avail)
    fi
}

probeBlockDevice()
{
    # get all devices
    local _vfat=$(blkid -t TYPE=vfat -o device)
    local _ext2=$(blkid -t TYPE=ext2 -o device)
    local _ext3=$(blkid -t TYPE=ext3 -o device)
    local _ext4=$(blkid -t TYPE=ext4 -o device)
    local _ntfs=$(blkid -t TYPE=ntfs -o device)

    #remove ro device
    local _i=0
    #vfat can support file > 4GB
    for dev in $_ext2 $_ext3 $_ext4 $_ntfs; do
        if [ "$dev" != "$ROOT_DEV" -a $(blockdev --getro $dev) -eq 0 -a -z "$(echo -n $dev | grep loop)" ]; then
            if [ $(blockDevGetFree $dev) -lt $min_temp_size ]; then
                continue;
            fi
            block_device_name[$_i]=$dev
            block_device_type[$_i]=$(blkid -s TYPE -o value $dev)
            block_device_total_size[$_i]=$(blockdev --getsize64 $dev)
            block_device_free_size[$_i]=$(blockDevGetFree $dev)
            block_device_label[$_i]=$(blkid -s LABEL -o value $dev)
            _i=$(($_i+1))
        fi
    done
}

makeOptions()
{
    for (( i=0; i<${#block_device_name[@]}; i++ )); do
        options[$i]="${block_device_name[$i]} ${block_device_type[$i]} ${block_device_free_size[$i]} ${block_device_total_size[$i]} ${block_device_label[$i]}"
    done

}

makeTempRoot()
{
    local _temp_device=$1
    local _ready
    local _temp_dir_prefix=`date +%Y-%m-%d-%H-%m-%S`
    mkdir -p /mnt/temproot
    if [ -d /mnt/temproot -a -w /mnt/temproot ]; then
        mount $_temp_device -t auto -orw /mnt/temproot &&
        mkdir -p /mnt/temproot/$_temp_dir_prefix &&
        _ready="yes"

        if [ "$_ready" = "yes" ]; then
            echo -n /mnt/temproot/$_temp_dir_prefix
        fi
    fi
}


closeTempRoot()
{
    local _temp_device=$1
    umount $_temp_device && rm -rf /mnt/temproot
}

clearRootImage()
{
    local _root_img=$1
    local _ready
    mkdir -p /mnt/tempfs
    if [ -d /mnt/tempfs -a -w /mnt/tempfs ]; then
        mount -t auto -orw,loop $_root_img /mnt/tempfs &&
        _ready="yes"

        if [ "$_ready" = "yes" ]; then
            cat >> /mnt/tempfs/clearroot.sh << FOE
#!/bin/sh
mount -t proc proc /proc
if [ ! -c /dev/zero ]; then
    mknod /dev/zero c 1 5
fi
if [ ! -c /dev/null ]; then
    mknod /dev/null c 1 3
fi

if [ ! -c /dev/urandom ]; then
    mknod /dev/urandom c 1 9
fi
if [ ! -c /dev/random ]; then
    mknod /dev/random c 1 8
fi
/sbin/load_policy -i
dnf clean all
dnf clean all
rpm --rebuilddb
rpm --rebuilddb
rm -rf /.liveimg-configured
rm -rf /.liveimg-late-configured
userdel -r -f liveuser 
rm -rf /home/liveuser
rm -rf /root
mkdir /root
chmod ug-w /root
chmod ug+rx /root
chmod o-rwx /root
cp /etc/skel/.bash_profile /root
cp /etc/skel/.bashrc /root
cp /etc/skel/.bash_profile /root
passwd -d root
passwd -l root
rm -rf /tmp/*
rm -rf /var/tmp/*
rm -rf /var/log/journal/*
find /var/log -type f -exec /bin/cp /dev/null {} \;
rm -rf /var/cache/fontconfig/*
rm -rf /var/cache/yum/*
rm -rf /var/cache/man/*
rm -rf /run/*
rm -rf /var/lib/gdm
rm -rf /var/spool/mail/*
mkdir -p /var/lib/gdm
chown -R gdm.gdm /var/lib/gdm
chmod o+t /var/lib/gdm
chmod ug+rwx /var/lib/gdm
chmod o-rwx /var/lib/gdm
restorecon -R /var/lib/gdm
rm -rf /var/log/journal/*
rm -rf /dev/*
echo '# edit by merger overlay' > /etc/fstab
echo '/dev/root  /         ext4    defaults,noatime 0 0' >> /etc/fstab
echo 'devpts     /dev/pts  devpts  gid=5,mode=620   0 0' >> /etc/fstab
echo 'tmpfs      /dev/shm  tmpfs   defaults         0 0' >> /etc/fstab
echo 'proc       /proc     proc    defaults         0 0' >> /etc/fstab
echo 'sysfs      /sys      sysfs   defaults         0 0' >> /etc/fstab
sed -i 's/AutomaticLoginEnable=True/AutomaticLoginEnable=False/' /etc/gdm/custom.conf
sed -i 's/AutomaticLogin=liveuser//' /etc/gdm/custom.conf
rm -rf /var/lib/AccountsService/users/liveuser
/sbin/restorecon -v /etc/passwd /etc/passwd-
/sbin/restorecon -v /etc/group /etc/group-
/sbin/restorecon -v /etc/shadow /etc/shadow-
/sbin/restorecon -Rv /root
/sbin/restorecon -Rv /var/lib
/sbin/restorecon -Rv /var/log
umount /proc
FOE
            mount -o bind /sys /mnt/tempfs/sys
            chroot /mnt/tempfs/ sh -x clearroot.sh
            umount /mnt/tempfs/sys/fs/selinux
            umount /mnt/tempfs/sys/
            rm -rf /mnt/tempfs/clearroot.sh
            echo "Cleaning empty space, this will take long time, please be patient"
            dd if=/dev/zero of=/mnt/tempfs/big-empty-file
            rm -rf /mnt/tempfs/big-empty-file
            umount /mnt/tempfs && rm -rf /mnt/tempfs && return 0
        fi
    fi

    return 1
}

probeBlockDevice

if [ ${#block_device_name[@]} -eq 0 ]; then
    echo "No block device available for temp space, quit!"
    exit 0
fi

makeOptions

title="Select a device for temp space, (hide device which min free space < $min_temp_size)"
prompt="Pick an option:"
echo $title
PS3="$prompt "
select_device=""
select opt in "${options[@]}" "Quit"; do
    case "$REPLY" in
    $(( ${#options[@]} + 1)) ) exit 0; break;;
    [0-9] ) select_device=${block_device_name[$(($REPLY - 1))]}; break;;
    *) echo "Invalid options"; continue;;
    esac
done

if [ -z "$select_device" ]; then
   exit 0
fi

echo Selected $select_device for temp space

echo "Making temp space"
temp_root_dir=$(makeTempRoot "$select_device")

if [ -z "$temp_root_dir" ]; then
    echo "Make temp root directory failed, quit!"
    exit 1
fi 

echo "Temp space is $temp_root_dir"

echo "Cloning root image to $temp_root_dir/fsimg/LiveOS/ext3fs.img"
mkdir -p "$temp_root_dir/fsimg/LiveOS/" &&
pv -tpreb -i 2 $ROOT_DEV | dd of="$temp_root_dir/fsimg/LiveOS/ext3fs.img" &&
echo "Done"

fsck.ext4 -y "$temp_root_dir/fsimg/LiveOS/ext3fs.img"

echo "Clearing root image"
clearRootImage $temp_root_dir/fsimg/LiveOS/ext3fs.img &&
echo "Done"

echo "Making new squash fs as $temp_root_dir/${squash_image}"
mksquashfs "$temp_root_dir/fsimg/" "$temp_root_dir/${squash_image}" -b 1M -comp xz -Xbcj x86 -Xdict-size '75%' &&
echo "Done"

if [ ! -f "$temp_root_dir/${squash_image}" ]; then
    "Run mksquash fail, quit!"
    exit 1
fi


echo "Backing up old img as $temp_root_dir/backup/${squash_image}"
mkdir -p "$temp_root_dir/backup"
pv -tpreb -i 2 $live_root_dir/${live_dir}/${squash_image} | dd of="$temp_root_dir/backup/${squash_image}"
rm -f $live_root_dir/${live_dir}/${squash_image}
rm -f $live_root_dir/${live_dir}/osmin.img
echo "Done"


echo "Copying new img as $live_root_dir/${live_dir}/${squash_image}"
pv -tpreb -i 2 "$temp_root_dir/${squash_image}" | dd of=$live_root_dir/${live_dir}/${squash_image}
rm -rf "$temp_root_dir/${squash_image}"
rm -rf "$temp_root_dir/osmin.img"
rm -rf "$temp_root_dir/fsimg"
echo "Done"


closeTempRoot $select_device

pushd .
echo "Creating MD5 sum"
cd $live_root_dir/${live_dir}/ && 
md5sum -b ${squash_image} > ${squash_image}.md5 &&
echo "Done"
popd

sync

echo "FINISH!!"
echo "Backup is $temp_root_dir/backup"
echo "Remember clear overlay manually after reboot"
echo "1. test new squash image with no overlay"
echo "2. remove ${live_dir}/overlay-xxx-xxx"
exit 0
