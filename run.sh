#!/bin/bash

make
gnome-terminal --working-directory=$PWD/Server -e "bash -c \"./run; exec bash\""
gnome-terminal --working-directory=$PWD/Client -e "bash -c \"./client; exec bash\""
