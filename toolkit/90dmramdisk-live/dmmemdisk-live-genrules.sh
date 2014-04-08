#!/bin/sh
# -*- mode: shell-script; indent-tabs-mode: nil; sh-basic-offset: 4; -*-
# ex: ts=8 sw=4 sts=4 et filetype=sh
/sbin/initqueue --settled --onetime --unique /sbin/dmmemdisk-live-root "${root}"
