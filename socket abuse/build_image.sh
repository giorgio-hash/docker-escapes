#!/bin/bash

docker build -t python_env:2 --build-arg dock_group=$(getent group docker | cut -d ":" -f 3) .
