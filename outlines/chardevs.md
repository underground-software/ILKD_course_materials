# chardevs

1. Learning Objective: Understand the purpose of a Linux character device

1. Overview

    1. What is a character device?

    1. Why do we want to use character devices

    1. How does the user interact with character devices?

    1. How does a kernel module implement a character device?

1. What is a character device?

    1. Character by character interface with the kernel

    1. An entry in /dev

    1. Streamed, not buffered

    1. Something like a file

    1. Usually implemented by a kernel module

1. How does the user interact with character devices?

    1. `ls -l /dev`

    1. `/proc/devices`

    1. `cat`, `echo`

    1. `mknod`

    1. Same way as other files

1. How does a kernel module implement a character device

    1. Overview

        1. Need major and minor numbers

        1. Need file operations

        1. Register device with cdev subsystem

        1. Implement open, close, read, write, and maybe more

    1. Start from module stub

    1. init and exit are modified

        1. Choose major and minors

        [`dev_t`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/types.h#L21)

        [`MAJOR`, `MINOR`, and `MKDEV`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/kdev_t.h#L10)

        [`alloc_chrdev_region`](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L236)

        [`register_chrdev_region`](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L200)

        [`unregister_chrdev_region`](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L311)

        1. Add/remove device from system

        [`struct cdev`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/cdev.h#L14)

        [`struct cdev_alloc`](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L640)

        [`cdev_init`](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L658)

        [`cdev_add`](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L479)

        [`cdev_del`](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L601)

        1. Implement the file operations

        [`struct file operations`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fs.h#L1774)

        [`struct inode`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fs.h#L608)

        [`struct file`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/fs.h#L961)

        [`copy_to_user()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/uaccess.h#L188)

1. Summary

    1. A character device implements a character-by-character interface with the kernel

    1. This is a common interface for device drivers

    1. Kernel modules can implement a character device

    1. A character device is identified by a major and minor number

    1. Character devices are generally located in `/dev`

    1. Character devices are labeled by 'c' in `ls -l /dev` output

    1. The `/proc/devices` file contains a list of major numbers and character devices

    1. Use open, close, read, write, and more to interact with the device
