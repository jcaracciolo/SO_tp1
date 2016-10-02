#!/bin/bash
tail -f Logs/$(ls Logs -t | grep 'log*' | head -n1) 2> /dev/null
