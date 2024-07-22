# chardevs2

1. Learning objecive: deepen and enrich your undestanding of character devices


* write, ioctl, lseek
* character class
* struct device
* struct device, 

Purpose:

* Represents any physical or virtual device in the Linux system
* Provides a unified way to manage and interact with devices


Key components:

* Device name
* Parent device pointer
* Device type
* Device class
* Device attributes
* Power management information
* DMA operations
* Device driver pointer


Relationship to other structures:

* Often embedded in larger, more specific device structures
* Linked to struct bus_type, which represents the bus the device is on
* Connected to struct device_driver, which represents the driver for the device

## Resources:

* [alloc_chrdev_region()](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L236)
* [cdev_init()](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L658)
* [cdev_add()](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L469)
* [cdev_del()](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L601)
* [unregister_chrdev_region()](https://elixir.bootlin.com/linux/v6.5/source/fs/char_dev.c#L311)
* [device_create()](https://elixir.bootlin.com/linux/v6.5/source/drivers/base/core.c#L4334)
* [device_register()](https://elixir.bootlin.com/linux/v6.5/source/drivers/base/core.c#L3704)
* [dev_set_drvdata()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device.h#L887)
* [device_destroy()](https://elixir.bootlin.com/linux/v6.5/source/drivers/base/core.c#L4397)
* [struct device](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device.h#L676)
* [struct device_driver](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/driver.h#L96)
* [struct class](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L52)
* [class_create()](https://elixir.bootlin.com/linux/v6.5/source/drivers/base/class.c#L256)
* [class_destroy()](https://elixir.bootlin.com/linux/v6.5/source/drivers/base/class.c#L289)
* [class_create_file()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L191)
* [class_remove_file()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L197)
* [struct class_atribute](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L171)
* [jiffies defined](https://elixir.bootlin.com/linux/v6.5/source/include/linux/jiffies.h#L80)
* [get_jiffies_64()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/jiffies.h#L85)

## The MIDI Format
* Encodes musical information in events, rather than a stream of audio
* Live MIDI processes events when they're recieved, MIDI files place deltas in between messages
* Deltas are variable-width, with 7 bits of information 
and a leading bit indicating whether the next byte is a delta or a message
* Messages either:
  * are **event messages** which directly communicate musical information (note on, note off, pitch bend, etc) 
  * are **meta messages** which contain things like time signature and tempo information, lyrics, or track names
* A MIDI file consists of: 
  * a global header
  * some number of track headers (in our case 1)
  * a series of meta-messages and note/event messages separated by deltas

More on MIDI:
* http://midi.teragonaudio.com
* https://faydoc.tripod.com/formats/mid.htm

## MIDI CharDev
* A series of write calls are made to `kkey000`-`kkey127`, with each one handling one note
* Each write call writes a single byte--`1` for note on, and `0` for note off
* After these calls have been made, a read call produces a midi file that can be played
* ioctl clears the track, removing the effect of the previous write calls

## demos
* cat /sys/class/kkey/kkey006/dev
* sizeof without parens discouraged upstream
* llseek standard implementations:
* [generic_file_llseek_size()](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L86)
* [fixed_size_llseek()](https://elixir.bootlin.com/linux/v6.5/source/fs/read_write.c#L162)
* [__ATTR()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/sysfs.h#L101)
* [kernel/ioctl.h (_IO*)](https://elixir.bootlin.com/linux/v6.5/source/arch/alpha/include/uapi/asm/ioctl.h#L48)
* example ioctl cmd #defines from claude:
```c
#include <linux/ioctl.h>
#define MYDEVICE_IOC_MAGIC  'k'
#define MYDEVICE_IOCRESET    _IO(MYDEVICE_IOC_MAGIC, 0)
#define MYDEVICE_IOCSQUALITY _IOW(MYDEVICE_IOC_MAGIC, 1, int)
#define MYDEVICE_IOCGQUALITY _IOR(MYDEVICE_IOC_MAGIC, 2, int)
```
# Locking (post L15)
[struct mutex](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L64)


[#define DEFINE_MUTEX()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L116)

[mutex_init()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L116)

[(debug) mutex_destroy()](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex-debug.c#L100)

[mutex_destroy()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L90)

[mutex_lock()](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L281)

[mutex_trylock()](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L1081)

[mutex_lock_interuptible()](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L977)

devres

struct bus_type

miscdevice, device file




![xxx](/slides/kkey_pics/ima_68827b0.jpeg)
![xxx](/slides/kkey_pics/ima_845a12b.jpeg)
![xxx](/slides/kkey_pics/ima_9f2cd16.jpeg)
![xxx](/slides/kkey_pics/ima_9f30b4f.jpeg)
![xxx](/slides/kkey_pics/ima_b86fe21.jpeg)
