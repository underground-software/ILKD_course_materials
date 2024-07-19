# chardevs2

write, ioctl, lseek

character class

struct device, devres


struct device, struct bus_type


claude.ai on struct device:

Purpose:

Represents any physical or virtual device in the Linux system
Provides a unified way to manage and interact with devices


Key components:

Device name
Parent device pointer
Device type
Device class
Device attributes
Power management information
DMA operations
Device driver pointer


Relationship to other structures:

Often embedded in larger, more specific device structures
Linked to struct bus_type, which represents the bus the device is on
Connected to struct device_driver, which represents the driver for the device

1. resources:

[device_create()](https://elixir.bootlin.com/linux/v6.5/source/drivers/base/core.c#L4334)

[device_register()](https://elixir.bootlin.com/linux/v6.5/source/drivers/base/core.c#L3704)

[dev_set_drvdata()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device.h#L887)

[device_destroy()](https://elixir.bootlin.com/linux/v6.5/source/drivers/base/core.c#L4397)

[struct device](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device.h#L676)

[struct device_driver](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/driver.h#L96)

[struct class](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L52)

[class_create()](https://elixir.bootlin.com/linux/v6.5/source/drivers/base/class.c#L256)

[class_destroy()](https://elixir.bootlin.com/linux/v6.5/source/drivers/base/class.c#L289)

[class_create_file()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L191)

[class_remove_file()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L197)

[struct class_atribute](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L171)

[struct mutex](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L64)

[#define DEFINE_MUTEX()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L116)

[mutex_init()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L116)

[(debug) mutex_destroy()](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex-debug.c#L100)

[mutex_destroy()](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L156)

[mutex_lock()](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L281)

[mutex_trylock()](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L1081)

[mutex_lock_interuptible()](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L977)

[mutex_unlock()](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L537)


NEXT:

miscdevice
device_create_file
device_destroy_file
