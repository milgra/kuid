#!/usr/bin/env bash

PIPE_IN="/tmp/wfl"

rm -f $PIPE_IN
mkfifo $PIPE_IN
exec 3<>$PIPE_IN

./build/kuid -v <&3 | while IFS= read -r line; do
    echo $line
    words=($line)
    if [ ${words[0]} = "event" ]
    then
	# init events
	if [ ${words[1]} = "init" ]
	then
	    # init layer
	    echo "create layer width 300 height 295 anchor rt margin 10" >&3
	    echo "load html src example/wfl/main.html" >&3

	    echo "set text div item0label value GlobalProtect" >&3
	    echo "set text div item0number value 0" >&3
	    echo "set text div item1label value Citrix" >&3
	    echo "set text div item1number value 1" >&3
	    echo "set text div item2label value Terminal" >&3
	    echo "set text div item2number value 2" >&3
	    echo "set text div item3label value Nautilus" >&3
	    echo "set text div item3number value 3" >&3
	    echo "set text div item4label value Chromium" >&3
	    echo "set text div item4number value 4" >&3
	    echo "set text div item5label value LibreOffice" >&3
	    echo "set text div item5number value 5" >&3
	    echo "set text div item6label value MMFM" >&3
	    echo "set text div item6number value 6" >&3
	    echo "set text div item7label value VMP" >&3
	    echo "set text div item7number value 7" >&3

	# button events
	elif [ ${words[1]} == "state" ]
	then
	    if [ ${words[3]} == "item0labelback" ]
	    then
		swaymsg exec globalprotect launch-ui
		echo "toggle visibility" >&3
	    elif [ ${words[3]} == "item1labelback" ]
	    then
		swaymsg exec /opt/Citrix/ICAClient/selfservice
		echo "toggle visibility" >&3
	    elif [ ${words[3]} == "item2labelback" ]
	    then
		swaymsg exec foot
		echo "toggle visibility" >&3
	    elif [ ${words[3]} == "item3labelback" ]
	    then
		swaymsg exec "nautilus --new-window"
		echo "toggle visibility" >&3
	    elif [ ${words[3]} == "item4labelback" ]
	    then
		swaymsg exec chromium
		echo "toggle visibility" >&3
	    elif [ ${words[3]} == "item5labelback" ]
	    then
		swaymsg exec libreoffic
		echo "toggle visibility" >&3
	    elif [ ${words[3]} == "item6labelback" ]
	    then
		swaymsg exec mmfm
		echo "toggle visibility" >&3
	    elif [ ${words[3]} == "item7labelback" ]
	    then
		swaymsg exec vmp
		echo "toggle visibility" >&3
	    elif [ ${words[3]} == "item8labelback" ]
	    then
		swaymsg exec system-config-printer
		echo "toggle visibility" >&3
	    fi
	fi
    fi

done
