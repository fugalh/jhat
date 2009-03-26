#! /bin/sh

# A unique grep pattern for your joystick. See /proc/bus/input/devices.
name="YOKE"

# The left/right axis, and the up/down axis. Use jstest to find this out.
xaxis=5
zaxis=6

# Executable paths
jhat=~/bin/jhat
xplane=~/xplane/X-Plane-athlon-xp

## end configuration

for d in /sys/class/input/js*; do
    if grep -q "$name" $d/device/name 2>/dev/null; then
	dev=/dev/input/`basename $d`
    fi
done

if [ -z "$dev" ]; then
    echo "Alas, the joystick to match '$name' was not found."
    exit 1
else
    echo "Joystick matching '$name' is $dev"
fi

$jhat $dev $xaxis $zaxis &
pid=$!

cd `dirname "$xplane"`
"$xplane"

kill $pid
