#!/bin/bash

dd if=/dev/random of=/dev/mapper/dmp1 bs=4k count=1
dd of=/dev/null if=/dev/mapper/dmp1 bs=4k count=1

# Получение статистики
cat /sys/kernel/dmp_info/dmp_sysfs

# Сброс статистики
echo 1 > /sys/kernel/dmp_info/dmp_sysfs

