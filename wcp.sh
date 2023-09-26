#!/usr/bin/env bash

PIPE_IN=$(mktemp -u)
mkfifo $PIPE_IN
exec 3<>$PIPE_IN
rm $PIPE_IN

./build/kuid -v <&3 | while IFS= read -r line; do
    for word in $line; do
	if [ $word = "Kinetic" ]
	then
	    echo "create layer" >&3
	    echo "load html" >&3
	    echo "third line" >&3
	    break
	elif [ $word = "event" ]
	then
	    echo $line
	    break
	fi
    done
done
