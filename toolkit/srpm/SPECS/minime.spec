Summary: MINIME Live System
Name: minime
Release: 1.0
Version: 4.0
License: GPLv2+
Group: System Environment/Base

%ifarch x86_64
%define _archflag -m64
%endif

%ifarch %{ix86}
%define _archflag -m32
%endif

%define cde_version 2.2.4
%define cde_zh_CN_version 1.0.0
%define dwipe_version 2.1.1
%define plymouth_themes_version 0.8.9
%define dracut_patch_version 1.0.0
%define fonts_version 1.0.0
%define skels_version 1.0.0
%define wallpapers_version 1.0.0
%define livesys_version 1.0.0
%define master_boot_code_version 1.22.0
%define gfxboot_version 4.5.6
%define gfxboot_themes_version 1.0.0
%define grub2_themes_version 1.0.0
%define misc_tools_version 1.0.0

Source0:	package.list
Source1:	pack.sh
Source2:	deploy.sh
Source3:	dwipe-%{dwipe_version}.tar.xz
Source4:	cde-%{cde_version}.tar.xz
Source5:	cde-zh_CN-%{cde_zh_CN_version}.tar.xz
Source6:	checkout-cde.sh
Source7:	cde.desktop
Source8:	startxsession.sh
Source9:	cde.ld.so.conf
Source10:	plymouth-themes-%{plymouth_themes_version}.tar.xz
Source11:	dracut-patch-%{dracut_patch_version}.tar.xz
Source12:	fonts-%{fonts_version}.tar.xz
Source13:	skels-%{skels_version}.tar.xz
Source14:	wallpapers-%{wallpapers_version}.tar.xz
Source15:	livesys-%{livesys_version}.tar.xz
Source16:	master-boot-code-%{master_boot_code_version}.tar.xz
Source17:	gfxboot-%{gfxboot_version}.tar.xz
Source18:	gfxboot-themes-%{gfxboot_themes_version}.tar.xz
Source19:	grub2-themes-%{grub2_themes_version}.tar.xz
Source20:	misc-tools-%{misc_tools_version}.tar.xz


Patch0:		cde-ansi.patch
Patch1:		cde-install.patch
Patch2:		cde-zh-cn.patch 

%debug_package

%description
MINIME Live System

%package cde
Summary: CDE is the Common Desktop Environment from The Open Group.
Group: User Interface/Desktops
BuildRequires:       xorg-x11-xbitmaps
BuildRequires:       xorg-x11-proto-devel
BuildRequires:       chrpath
BuildRequires:       ncompress
BuildRequires:       libXp-devel
BuildRequires:       libXt-devel
BuildRequires:       libXmu-devel
BuildRequires:       libXft-devel
BuildRequires:       libXinerama-devel
BuildRequires:       libXpm-devel 
BuildRequires:       motif 
BuildRequires:       motif-devel
BuildRequires:       libXaw-devel
BuildRequires:       libX11-devel
BuildRequires:       libtirpc-devel
BuildRequires:       libjpeg-turbo-devel
BuildRequires:       freetype-devel
BuildRequires:       openssl-devel


%description cde
CDE is the Common Desktop Environment from The Open Group.

%package dwipe
Summary: Disk wipe tool
Group: System Environment/Base

%description dwipe
Disk wipe tool

%package plymouth-themes
Summary: Plymouth theme for minime
Group: System Environment/Base

%description plymouth-themes
Plymouth theme for minime

%package dracut-patch
Summary: Patchs for dracut
Group: System Environment/Base

%description dracut-patch
Patchs for dracut

%package fonts
Summary: Fonts for minime
Group: System Environment/Base

%description fonts
Fonts for minime

%package skels
Summary: preset user profiles
Group: System Environment/Base

%description skels
Wallpapers for minime

%package wallpapers
Summary: preset user profiles
Group: System Environment/Base

%description wallpapers
Wallpapers for minime

%package livesys
Summary: livesys script for minime
Group: System Environment/Base

%description livesys
livesys script for minime

%package master-boot-code
Summary: i386 Master Boot Record Code
Group: System/Boot

%description master-boot-code
The i386 master boot code is code that, after copied to the MBR of the
boot disk, loads and starts the boot sector of the active partition.

%package gfxboot
Summary:        Graphical Boot Logo for GRUB, LILO and SYSLINUX
Group:          System/Boot

%description gfxboot
Tools to configure the graphics for your GRUB, LILO or SYSLINUX bootloader.

%package gfxboot-themes
Summary:        Graphical Boot Logo for GRUB, LILO and SYSLINUX
Group:          System/Boot

%description gfxboot-themes
Tools to configure the graphics for your GRUB, LILO or SYSLINUX bootloader.

%package grub2-themes
Summary:        grub2 theme for minime
Group:          System/Boot

%description grub2-themes
grub2 theme for minime


%package misc-tools
Summary:        misc tools
Group:          System/Boot

%description misc-tools
misc tools

%prep
rm -rf %{name}-%{version}
mkdir %{name}-%{version}
tar -Jxvmf %{_sourcedir}/cde-%{cde_version}.tar.xz -C %{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/cde-zh_CN-%{cde_zh_CN_version}.tar.xz -C %{_builddir}/%{name}-%{version}/cde-%{cde_version} --strip-components=1
tar -Jxvmf %{_sourcedir}/dwipe-%{dwipe_version}.tar.xz -C %{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/plymouth-themes-%{plymouth_themes_version}.tar.xz -C%{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/dracut-patch-%{dracut_patch_version}.tar.xz  -C%{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/fonts-%{fonts_version}.tar.xz -C%{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/skels-%{skels_version}.tar.xz -C%{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/wallpapers-%{wallpapers_version}.tar.xz -C%{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/livesys-%{livesys_version}.tar.xz -C%{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/master-boot-code-%{master_boot_code_version}.tar.xz -C%{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/gfxboot-%{gfxboot_version}.tar.xz -C%{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/gfxboot-themes-%{gfxboot_themes_version}.tar.xz -C%{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/grub2-themes-%{grub2_themes_version}.tar.xz -C%{_builddir}/%{name}-%{version}
tar -Jxvmf %{_sourcedir}/misc-tools-%{misc_tools_version}.tar.xz -C%{_builddir}/%{name}-%{version}


%patch0 -p0
%patch1 -p0
%patch2 -p0

%build
cd %{_builddir}/%{name}-%{version}/cde-%{cde_version}
%{__make} World.dev BOOTSTRAPCFLAGS="%{optflags} %{_archflag}"
%{__make} World.doc BOOTSTRAPCFLAGS="%{optflags} %{_archflag}"

cd %{_builddir}/%{name}-%{version}/dwipe-%{dwipe_version}
%{__make}

cd %{_builddir}/%{name}-%{version}/plymouth-themes-%{plymouth_themes_version}
%{__make}

cd %{_builddir}/%{name}-%{version}/master-boot-code-%{master_boot_code_version}
%{__make}

cd %{_builddir}/%{name}-%{version}/gfxboot-%{gfxboot_version}
export SUSE_ASNEEDED=0
%{__make} DESTDIR=%{buildroot}
%{__make} X11LIBS=%{_prefix}/X11R6/%{_lib}
%{__make} doc

cd %{_builddir}/%{name}-%{version}/misc-tools-%{misc_tools_version}
./build.sh

%install
## install cde
cd %{_builddir}/%{name}-%{version}/cde-%{cde_version}/admin/IntegTools/dbTools
./installCDE -s %{_builddir}/%{name}-%{version}/cde-%{cde_version} \
	-t %{_builddir}/%{name}-%{version}/cde-%{cde_version}/tars -nocompress
DTTAR="$(find %{_builddir}/%{name}-%{version}/cde-%{cde_version}/tars -name dt.tar)"
tar -C %{buildroot} -xpsvf ${DTTAR}
chmod -R u+w %{buildroot}
find %{buildroot}%{_prefix}/dt/bin -type f | \
    grep -v -E "(startxsession\.sh|Xsession|dtappintegrate|dtdocbook|dterror\.ds|dtfile_error|dthelpgen\.ds|dthelpprint\.sh|dthelptag|dtinfogen|dtlp|dtsession_res|ttrmdir)" | \
    xargs chrpath -d
find %{buildroot}%{_prefix}/dt/lib -type f -name "lib*.so*" | xargs chrpath -d
find %{buildroot}%{_prefix}/dt/lib/dtudcfonted -type f -name "dt*" | xargs chrpath -d
chrpath -d %{buildroot}%{_prefix}/dt/dthelp/dtdocbook/instant
chrpath -d %{buildroot}%{_prefix}/dt/dthelp/dtdocbook/xlate_locale
chrpath -d %{buildroot}%{_prefix}/dt/infolib/etc/nsgmls

# Create other required directories.
mkdir -p %{buildroot}%{_sysconfdir}/dt
mkdir -p %{buildroot}%{_localstatedir}/dt

# Create other files
mkdir -p %{buildroot}%{_prefix}/dt/bin/
mkdir -p %{buildroot}%{_datadir}/xsessions/
mkdir -p %{buildroot}%{_sysconfdir}/ld.so.conf.d
cp %{_sourcedir}/startxsession.sh %{buildroot}%{_prefix}/dt/bin/
cp %{_sourcedir}/cde.desktop %{buildroot}%{_datadir}/xsessions/
cp %{_sourcedir}/cde.ld.so.conf    %{buildroot}%{_sysconfdir}/ld.so.conf.d/cde.conf


## install dwipe
mkdir -p  %{buildroot}%{_bindir}
cp %{_builddir}/%{name}-%{version}/dwipe-%{dwipe_version}/dwipe/dwipe \
	%{buildroot}%{_bindir}
mkdir -p  %{buildroot}/lib/dracut/modules.d
cp -r %{_builddir}/%{name}-%{version}/dwipe-%{dwipe_version}/dracut/90dwipe \
	%{buildroot}/lib/dracut/modules.d

## install plymouth-themes
mkdir -p  %{buildroot}%{_libdir}/plymouth/
cp %{_builddir}/%{name}-%{version}/plymouth-themes-%{plymouth_themes_version}/scriptx/scriptx.so \
	%{buildroot}%{_libdir}/plymouth
mkdir -p  %{buildroot}%{_datadir}/plymouth/themes
cp -r %{_builddir}/%{name}-%{version}/plymouth-themes-%{plymouth_themes_version}/themes/minime \
	%{buildroot}%{_datadir}/plymouth/themes
mkdir -p  %{buildroot}/lib//dracut/modules.d
cp -r %{_builddir}/%{name}-%{version}/plymouth-themes-%{plymouth_themes_version}/dracut/50plymouth-fix \
	%{buildroot}/lib/dracut/modules.d

## install dracut-patch
mkdir -p %{buildroot}/lib/dracut/modules.d
cp -r %{_builddir}/%{name}-%{version}/dracut-patch-%{dracut_patch_version}/dracut/* \
	%{buildroot}/lib/dracut/modules.d
mkdir -p %{buildroot}%{_sysconfdir}
cp -r %{_builddir}/%{name}-%{version}/dracut-patch-%{dracut_patch_version}/etc/* \
	%{buildroot}%{_sysconfdir}

## install fonts
mkdir -p %{buildroot}%{_datadir}/fonts
mkdir -p %{buildroot}%{_datadir}/fontconfig/conf.avail
cp -r %{_builddir}/%{name}-%{version}/fonts-%{fonts_version}/minime \
	%{buildroot}%{_datadir}/fonts
cp %{_builddir}/%{name}-%{version}/fonts-%{fonts_version}/52-chinese-fonts.conf \
	%{buildroot}%{_datadir}/fontconfig/conf.avail
mkdir -p %{buildroot}%{_sysconfdir}/fonts/conf.d
ln -s %{_datadir}/fontconfig/conf.avail/52-chinese-fonts.conf \
	%{buildroot}%{_sysconfdir}/fonts/conf.d/52-chinese-fonts.conf

## install skels
mkdir -p %{buildroot}%{_sysconfdir}
cp -r %{_builddir}/%{name}-%{version}/skels-%{skels_version}/skel \
	%{buildroot}%{_sysconfdir}

## install wallpapers
mkdir -p %{buildroot}%{_datadir}
cp -r %{_builddir}/%{name}-%{version}/wallpapers-%{wallpapers_version}/* \
	%{buildroot}%{_datadir}

## install livesys
mkdir -p %{buildroot}%{_prefix}/lib/systemd/system
cp -r %{_builddir}/%{name}-%{version}/livesys-%{livesys_version}/* \
	%{buildroot}%{_prefix}/lib/systemd/system

## install master-boot-code
mkdir -p %{buildroot}%{_prefix}/lib/boot
mkdir -p %{buildroot}%{_sbindir}
install -c -p -m 644 %{_builddir}/%{name}-%{version}/master-boot-code-%{master_boot_code_version}/mbr512 \
	%{buildroot}%{_prefix}/lib/boot/master-boot-code
install -c -p -m 644 %{_builddir}/%{name}-%{version}/master-boot-code-%{master_boot_code_version}/mbr \
	%{buildroot}%{_prefix}/lib/boot/MBR
install -c -p -m 755 %{_builddir}/%{name}-%{version}/master-boot-code-%{master_boot_code_version}/fixmbr \
	%{buildroot}%{_sbindir}/fixmbr

## install gfxboot
cd %{_builddir}/%{name}-%{version}/gfxboot-%{gfxboot_version}
%{make_install}
mkdir -p %{buildroot}%{_datadir}/gfxboot
mkdir -p %{buildroot}%{_datadir}/doc/gfxboot/
mkdir -p %{buildroot}%{_mandir}/man8/
cat  %{_builddir}/%{name}-%{version}/gfxboot-%{gfxboot_version}/doc/gfxboot.8 | \
	gzip > %{buildroot}%{_mandir}/man8/gfxboot.8.gz
cp %{_builddir}/%{name}-%{version}/gfxboot-%{gfxboot_version}/doc/gfxboot.html \
	%{buildroot}%{_datadir}/doc/gfxboot/
cp %{_builddir}/%{name}-%{version}/gfxboot-%{gfxboot_version}/doc/gfxboot.txt \
        %{buildroot}%{_datadir}/doc/gfxboot/
cp -r %{_builddir}/%{name}-%{version}/gfxboot-%{gfxboot_version}/bin \
	%{buildroot}%{_datadir}/gfxboot
cp -r %{_builddir}/%{name}-%{version}/gfxboot-%{gfxboot_version}/test \
        %{buildroot}%{_datadir}/gfxboot

## install gfxboot-themes
mkdir -p %{buildroot}%{_datadir}/gfxboot/themes
cp -r %{_builddir}/%{name}-%{version}/gfxboot-themes-%{gfxboot_themes_version}/themes \
        %{buildroot}%{_datadir}/gfxboot/themes

## install grub2-themes
mkdir -p %{buildroot}%{_datadir}/grub/themes
cp -r %{_builddir}/%{name}-%{version}/grub2-themes-%{grub2_themes_version}/themes \
        %{buildroot}%{_datadir}/grub/themes


## install misc-tools
cp -r %{_builddir}/%{name}-%{version}/misc-tools-%{misc_tools_version}/my_ply_client \
	%{buildroot}%{_bindir}
cp -r %{_builddir}/%{name}-%{version}/misc-tools-%{misc_tools_version}/i3-im \
        %{buildroot}%{_bindir}
cp -r %{_builddir}/%{name}-%{version}/misc-tools-%{misc_tools_version}/build_version \
        %{buildroot}%{_bindir}

%clean
rm -rf %{buildroot}

##################################################
%files cde
%defattr(-,root,root,-)
%{_sysconfdir}/ld.so.conf.d/cde.conf
%{_prefix}/dt
%{_localstatedir}/dt
%{_sysconfdir}/dt
%{_datadir}/xsessions/cde.desktop

%post cde
/usr/sbin/ldconfig
sed -i 's/RPCBIND_ARGS=.*/RPCBIND_ARGS="-i"/' /etc/sysconfig/rpcbind
systemctl enable rpcbind.service
systemctl start rpcbind.service

%postun cde
/usr/sbin/ldconfig
sed -i 's/RPCBIND_ARGS=.*/RPCBIND_ARGS=""/' /etc/sysconfig/rpcbind
systemctl stop rpcbind.service
systemctl disable rpcbind.service

##################################################
%files dwipe
/lib/dracut/modules.d/90dwipe
%{_bindir}/dwipe

#################################################
%files plymouth-themes
%{_libdir}/plymouth/scriptx.so
%{_datadir}/plymouth/themes/minime
/lib/dracut/modules.d/50plymouth-fix

##################################################
%files dracut-patch
/lib/dracut/modules.d/90merge-overlay
/lib/dracut/modules.d/90ntfs-live
/lib/dracut/modules.d/90minishell
/lib/dracut/modules.d/90libgcrypthmac
/lib/dracut/modules.d/90dmsquash-live/*
%{_sysconfdir}/minime.conf
%{_sysconfdir}/minime-minishell.conf

%post dracut-patch
patch -p0 < /lib/dracut/modules.d/90dmsquash-live/dmsquash-live-root.sh.patch
mv /etc/motd /etc/motd.BAK
ln -s /var/run/initramfs/live/boot/Version /etc/motd

##################################################
%files fonts
%{_datadir}/fonts/minime
%{_datadir}/fontconfig/conf.avail/*
%{_sysconfdir}/fonts/conf.d/*

###################################################
%files skels
%{_sysconfdir}/skel/.bash*
%{_sysconfdir}/skel/.config
%{_sysconfdir}/skel/.emacs
%{_sysconfdir}/skel/.i3
%{_sysconfdir}/skel/.kde
%{_sysconfdir}/skel/.kshrc
%{_sysconfdir}/skel/.local
%{_sysconfdir}/skel/.mozilla
%{_sysconfdir}/skel/.xemacs
%{_sysconfdir}/skel/.zshrc
%{_sysconfdir}/skel/.Xdefaults

###################################################
%files wallpapers
%{_datadir}/plasma/*
%{_datadir}/backgrounds/*
%{_datadir}/wallpapers/*

##################################################
%files livesys
%{_prefix}/lib/systemd/system/livesys.service

%post livesys
chkconfig --del livesys
chkconfig --del livesys-late
mv /etc/init.d/livesys /etc/init.d/livesys.BAK
mv /etc/init.d/livesys-late /etc/init.d/livesys-late.BAK
systemctl enable livesys

####################################################
%files master-boot-code
%defattr(-,root,root)
%{_prefix}/lib/boot/master-boot-code
%{_prefix}/lib/boot/MBR
%{_sbindir}/fixmbr

###################################################
%files gfxboot
%defattr(-,root,root)
%{_sbindir}/gfxboot
%{_mandir}/man8/gfxboot.8.gz
%{_datadir}/gfxboot/bin
%{_datadir}/gfxboot/test
%{_datadir}/doc/gfxboot
%{_sbindir}/gfxboot-compile
%{_sbindir}/gfxboot-font
%{_sbindir}/gfxtest

####################################################
%files gfxboot-themes
%{_datadir}/gfxboot/themes

###################################################
%files grub2-themes
%{_datadir}/grub/themes

###################################################
%files misc-tools
%{_bindir}/my_ply_client
%{_bindir}/i3-im
%{_bindir}/build_version

%changelog
* Thu Feb 04 2016 Fedora Release Engineering <releng@fedoraproject.org> - 0.8.9-17.2013.08.14
- Rebuilt for https://fedoraproject.org/wiki/Fedora_24_Mass_Rebuild
