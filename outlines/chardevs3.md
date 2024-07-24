# chardevs3: the kernel screams

1. Learning objective: listen to the sound of a race condition

1. Overview

    1. Class files in sysfs

    1. Mutexes in the kernel

    1. Atomic types in the kernel

    1. Triggering a race

    1. Preventing the race


![xxx](/slides/kkey_pics/listen.jpeg)
![xxx](/slides/kkey_pics/time.jpeg)
![xxx](/slides/kkey_pics/cache.jpeg)
![xxx](/slides/kkey_pics/readwrite.jpeg)
![xxx](/slides/kkey_pics/simplified.jpeg)

[`class_create_file()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L191)

[`class_remove_file()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L197)

[`struct class_attribute`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/device/class.h#L171)

```c
	ssize_t (*show)(const struct class *class, const struct class_attribute *attr,
			char *buf);
	ssize_t (*store)(const struct class *class, const struct class_attribute *attr,
			 const char *buf, size_t count);
```

[`sysfs_emit()`](https://elixir.bootlin.com/linux/v6.5/source/fs/sysfs/file.c#L729)

[`offset_in_page()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mm.h#L2236)

1. Buffer checks done for us, write to start of a page

[`kstrtou8()`](https://elixir.bootlin.com/linux/v6.5/source/lib/kstrtox.c#L307)

1. Why not `*_from_user`?


[`#define DEFINE_MUTEX()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L116)

[`mutex_init()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L116)

[`(debug) mutex_destroy()`](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex-debug.c#L100)

[`mutex_destroy()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/mutex.h#L90)

[`mutex_lock()`](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L281)

[`mutex_trylock()`](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L1081)

[`mutex_lock_interuptible()`](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L977)

[`mutex_unlock()`](https://elixir.bootlin.com/linux/v6.5/source/kernel/locking/mutex.c#L537)


[`atomic{,64}_t, ATOMIC_INIT()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/types.h#L174)

[`atomic_read()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/atomic/atomic-instrumented.h#L30)

[`atomic64_read()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/atomic/atomic-instrumented.h#L1594)

[`atomic_set()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/atomic/atomic-instrumented.h#L65)

[`atomic64_set()`](https://elixir.bootlin.com/linux/v6.5/source/include/linux/atomic/atomic-instrumented.h#L1629)

[`sysconf(3)`, `_SC_NPROCESSORS_ONLN`](https://man7.org/linux/man-pages/man3/sysconf.3.html)

[`CPU_SET, CPU_ZERO`](https://man7.org/linux/man-pages/man3/CPU_SET.3.html)

[`sched_setaffinity(2)`](https://man7.org/linux/man-pages/man2/sched_setaffinity.2.html)

[`D_GNU_SOURCE`](https://man7.org/linux/man-pages/man7/feature_test_macros.7.html)
