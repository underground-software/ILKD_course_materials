#!/bin/sh

OFFS=$(midicsvpy race.mid /dev/stdout | grep -c off)
ONS=$(midicsvpy race.mid /dev/stdout | grep -c on)

if [ "$OFFS" != "128" -o "$ONS" != "128" ]; then
	echo "Race detected: offs $OFFS ons $ONS"
else
	echo "No race detected"
fi

