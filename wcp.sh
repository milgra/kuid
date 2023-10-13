#!/usr/bin/env bash

PIPE_IN=$(mktemp -u)
mkfifo $PIPE_IN
exec 3<>$PIPE_IN
rm $PIPE_IN

./build/kuid -v <&3 | while IFS= read -r line; do
    echo $line
    words=($line)
    if [ ${words[0]} = "event" ]
    then
	if [ ${words[1]} = "init" ]
	then
	    echo "create layer" >&3
	    echo "load html" >&3
	    echo "third line" >&3
	fi
    fi
done
