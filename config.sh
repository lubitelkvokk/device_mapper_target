#!/bin/bash
SIZE=4096

make
insmod dmp.ko
# setup
dmsetup create zero1 --table "0 $SIZE zero"
dmsetup create dmp1 --table "0 $SIZE dmp /dev/mapper/zero1"