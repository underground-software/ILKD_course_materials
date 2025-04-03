#!/bin/sh

dd if=/dev/zero of=diskimage.img bs=1M count=100
mkfs.ext4 diskimage.img
sudo mkdir /mnt/loopback
sudo mount -o loop diskimage.img /mnt/loopback
df -h /mnt/loopback
