#!/bin/bash

echo ########
echo disinstalla...
echo ########

apt-get purge -y docker-engine docker docker.io docker-ce docker-ce-cli docker-compose-plugin
apt-get autoremove -y --purge docker-engine docker docker.io docker-ce docker-compose-plugin
rm -rf /etc/apt/keyrings

echo ########
echo reinstalla...
echo ########

apt-get install ca-certificates curl gnupg

install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
chmod a+r /etc/apt/keyrings/docker.gpg
echo "deb [arch="$(dpkg --print-architecture)" signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu "$(. /etc/os-release && echo "$VERSION_CODENAME")" stable" | tee /etc/apt/sources.list.d/docker.list > /dev/null
apt-get update

echo y | dpkg -i containerd.io_1.2.0-1_amd64.deb
echo y | dpkg -i docker-ce-cli_18.09.1~3-0~ubuntu-bionic_amd64.deb
echo y | dpkg -i docker-ce_18.09.1~3-0~ubuntu-bionic_amd64.deb
echo y | apt-get install docker-buildx-plugin docker-compose-plugin

echo ########
echo "[v]fatto!"
echo ########
