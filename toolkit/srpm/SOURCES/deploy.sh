#!/bin/sh
. ./package.list
for i in `echo $packages`; do
tar -Jxvmf $i.tar.xz && rm -rf $i.tar.xz
done

