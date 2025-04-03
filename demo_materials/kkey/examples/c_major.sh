#!/bin/sh

echo 1 > /dev/kkey060
sleep 0.1
echo 1 > /dev/kkey064
sleep 0.1
echo 1 > /dev/kkey067
sleep 1
echo 0 > /dev/kkey060
echo 0 > /dev/kkey064
echo 0 > /dev/kkey067
