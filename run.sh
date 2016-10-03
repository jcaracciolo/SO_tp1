#!/bin/bash

make $1
gnome-terminal --working-directory=$PWD/Server -e "bash -c \"./run; exec bash\""
gnome-terminal --working-directory=$PWD/Client -e "bash -c \"./client; exec bash\""
gnome-terminal --working-directory=$PWD/Server -e "bash -c \"./log.sh; exec bash\""
gnome-terminal --working-directory=$PWD/Client -e "bash -c \"./user; exec bash\""
