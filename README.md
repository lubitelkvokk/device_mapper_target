# Guide
```Ubuntu-24.04```

```6.8.0-58-generic```

**Выдать права на исполнение**

Сборка и установка: ```sudo ./config.sh```

Тестирование: ```./test.sh```

Очистка: ```sudo ./clean.sh```

## Результат сборки c копированием блоков:
```shell
make -C /lib/modules/6.8.0-58-generic/build M=/path/to/module_dir modules
make[1]: Entering directory '/usr/src/linux-headers-6.8.0-58-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  You are using:           gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  CC [M]  /path/to/module_dir/dmp.o
  MODPOST /path/to/module_dir/Module.symvers
  CC [M]  /path/to/module_dir/dmp.mod.o
  LD [M]  /path/to/module_dir/dmp.ko
  BTF [M] /path/to/module_dir/dmp.ko
Skipping BTF generation for /path/to/module_dir/dmp.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-6.8.0-58-generic'
device-mapper: create ioctl on zero1  failed: Device or resource busy
Command failed.
1+0 records in
1+0 records out
4096 bytes (4.1 kB, 4.0 KiB) copied, 0.000358698 s, 11.4 MB/s
1+0 records in
1+0 records out
4096 bytes (4.1 kB, 4.0 KiB) copied, 0.000438385 s, 9.3 MB/s
```

## Проверка статистики
```shell
> cat /sys/kernel/dmp_info/dmp_sysfs 
read:
        reqs: 0
        avg size: 0
write:
        reqs: 0
        avg size:0
total:
        reqs: 0
        avg size:0
> dd if=/dev/random of=/dev/mapper/dmp1 bs=4k count=1
1+0 records in
1+0 records out
4096 bytes (4.1 kB, 4.0 KiB) copied, 0.0236389 s, 173 kB/s
> cat /sys/kernel/dmp_info/dmp_sysfs 
read:
        reqs: 112
        avg size: 4096
write:
        reqs: 1
        avg size:4096
total:
        reqs: 113
        avg size:4096
> dd of=/dev/null if=/dev/mapper/dmp1 bs=4k count=1
1+0 records in
1+0 records out
4096 bytes (4.1 kB, 4.0 KiB) copied, 0.0219914 s, 186 kB/s
> cat /sys/kernel/dmp_info/dmp_sysfs 
read:
        reqs: 114
        avg size: 4203
write:
        reqs: 1
        avg size:4096
total:
        reqs: 115
        avg size:4202
> dd of=/dev/null if=/dev/mapper/dmp1 bs=4k count=5
5+0 records in
5+0 records out
20480 bytes (20 kB, 20 KiB) copied, 0.0695697 s, 294 kB/s
> cat /sys/kernel/dmp_info/dmp_sysfs 
read:
        reqs: 122
        avg size: 5036
write:
        reqs: 1
        avg size:4096
total:
        reqs: 123
        avg size:5028
```
