#!/bin/sh

ip="192.168.42.25"
pudfile=/data/ftp/internal_000/Disco/academy/*.temp
while true; do
	exist=$(ls $pudfile 2> /dev/null)	
	if [ -z "$exist" ]; then
		i=1
	else
		tail -c -83 $pudfile | ./data/ftp/uavpal/bin/netcat-arm -u ${ip} 9999 -w1&
	fi
done
