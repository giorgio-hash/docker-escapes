#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]
then
    echo "uso: docker run -it [...] nome_container:versione_container  indirizzo  porta";
    exit 1
else
    unshare -UrmC ./find_root_cgroup.sh $1 $2
fi
