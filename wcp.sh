#!/usr/bin/env bash

PIPE_IN="/tmp/wcp"

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
	    echo "create layer width 300 height 225 anchor rt" >&3
	    echo "load html src example/wcp/main.html" >&3

	    # set volume
	    vol=$(pulsemixer --get-volume | awk '{print $1;}')
	    volrat=$(echo "scale=2 ; $vol / 100" | bc)
	    echo "set ratio div volslider value $volrat" >&3

	    # set brightness
	    act=$(brightnessctl g)
	    max=$(brightnessctl m)
	    lcdrat=$(echo "scale=2 ; $act / $max" | bc)
	    echo "set ratio div lcdslider value $lcdrat" >&3

	    # set bluetooth device's name
	    device=$(bluetoothctl info | sed -n 2p | awk '{for(i=2;i<=NF;i++) printf $i" "}')
	    if [ ${#device} -eq 0 ]
	    then
		device="No_device_connected"
	    fi
	    echo "set text div btoothlabel value $device" >&3

	    # set wifi network name
	    network=$(nmcli con show | awk 'NR==2 {print $1}')
	    echo "set text div wifilabel value $network" >&3

	    echo "set text div datelabel value $(date +%H:%M)" >&3
	    echo "set text div langlabel value EN_US" >&3

	elif [ ${words[1]} = "update" ]
	then
	    # set volume
	    vol=$(pulsemixer --get-volume | awk '{print $1;}')
	    volrat=$(echo "scale=2 ; $vol / 100" | bc)
	    echo "set ratio div volslider value $volrat" >&3

	    # set brightness
	    act=$(brightnessctl g)
	    max=$(brightnessctl m)
	    lcdrat=$(echo "scale=2 ; $act / $max" | bc)
	    echo "set ratio div lcdslider value $lcdrat" >&3

	    # set bluetooth device's name
	    device=$(bluetoothctl info | sed -n 2p | awk '{for(i=2;i<=NF;i++) printf $i" "}')
	    if [ ${#device} -eq 0 ]
	    then
		device="No_device_connected"
	    fi
	    echo "set text div btoothlabel value $device" >&3

	    # set wifi network name
	    network=$(nmcli con show | awk 'NR==2 {print $1}')
	    echo "set text div wifilabel value $network" >&3

	    echo "set text div datelabel value $(date +%H:%M)" >&3
	    echo "set text div langlabel value EN_US" >&3

	# slider events
	elif [ ${words[1]} == "ratio" ]
	then
	    if [ ${words[3]} == "volslider" ]
	    then
		# set default sinks volume
		vol=${words[7]}
		res=$(pactl set-sink-volume @DEFAULT_SINK@ $vol%)
	    elif [ ${words[3]} == "lcdslider" ]
	    then
		# set brightness
		lcd=${words[7]}
		res=$(brightnessctl set $lcd%)
	    fi

	# button events
	elif [ ${words[1]} == "state" ]
	then
	    if [ ${words[3]} == "mutebtn" ]
	    then
		# open pulseaudio
		swaymsg exec pavucontrol
	    elif [ ${words[3]} == "displaybtn" ]
	    then
		# open wdisplays
		swaymsg exec wdisplays
	    elif [ ${words[3]} == "wifibtn" ] || [ ${words[3]} == "wifilabelback" ]
	    then
		# swaymsg exec iwgtk
		swaymsg exec "foot nmtui"
	    elif [ ${words[3]} == "bluetoothbtn" ] || [ ${words[3]} == "bluetoothlabelback" ]
	    then
		swaymsg exec blueman-manager
	    elif [ ${words[3]} == "lockbtn" ]
	    then
		swaymsg exec swaylock
		# gnome-screensaver-command -l
	    elif [ ${words[3]} == "logoutbtn" ]
	    then
		sway exit
		# swaymsg exec loginctl terminate-user $USER
	    elif [ ${words[3]} == "suspendbtn" ]
	    then
		# sudo /sbin/zzz
		swaymsg exec systemctl suspend
	    elif [ ${words[3]} == "rebootbtn" ]
	    then
		# sudo /sbin/reboot
		swaymsg exec systemctl reboot
	    elif [ ${words[3]} == "shutdownbtn" ]
	    then
		# sudo /sbin/poweroff
		swaymsg exec systemctl poweroff
	    fi
	fi
    fi
done
