#!/bin/sh
. ./package.list
for i in `echo $packages`; do
tar -Jcvmf $i.tar.xz $i && rm -rf $i
done
