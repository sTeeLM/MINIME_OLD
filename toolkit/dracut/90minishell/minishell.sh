#!/bin/sh

if getargbool 0 rd.minishell ; then
    exec /sbin/init
fi
