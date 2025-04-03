#!/bin/sh

trap 'echo "$count attempts at race"' EXIT

count=0
while test $count -lt 100; do
count=$((count + 1))

./reset
./examples/race
cat /dev/kkey000 > kkey.mid

# from: pip install py_midicsv
OFFS=$(midicsvpy kkey.mid /dev/stdout | grep -c off)
ONS=$(midicsvpy kkey.mid /dev/stdout | grep -c on)

if [ "$OFFS" != "128" -o "$ONS" != "128" ]; then
	echo "**********************************"
	echo "**********************************"
	echo "Race detected: offs: $OFFS ons: $ONS"
	echo "**********************************"
	echo "**********************************"
	exit
fi

done

echo "[NO RACE DETECTED]"
