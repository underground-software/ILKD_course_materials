# chardevs3: the kernel screams

---

# Learning objective

Listen to the sound of a race condition and understand the fix

---

# Overview

1. Class files in sysfs

1. Mutexes in the kernel

1. Atomic types in the kernel

1. Triggering a race

1. Preventing the race

---

# Class files

1. Use `sysfs` to expose a kernel interface

1. Can be read-only, write-only, or read-write

1. More options exist beyond class files

```
    ->show() == read

    ->store() == write
```

---

# Class file API

[`class_create_file()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/device/class.h#L189)

[`class_remove_file()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/device/class.h#L195)

[`struct class_attribute`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/device/class.h#L169)

---

# Storing properly

[`sysfs_emit()`](https://elixir.bootlin.com/linux/v6.13/source/fs/sysfs/file.c#L756)

[`offset_in_page()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/mm.h#L2313)

1. Buffer checks done for us, write to start of a page

[`kstrtou8()`](https://elixir.bootlin.com/linux/v6.13/source/lib/kstrtox.c#L307)

1. Why not `*_from_user`?

---

# demo

`kkey` implements RW attributes for `velocity` and `slowdown`

---

# Locking: mutex API

Initialization can be static or dynamic

1. Static: [`#define DEFINE_MUTEX()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/mutex.h#L86)

1. Dynamic: [`mutex_init()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/mutex.h#L60)

Destruction is a no-op on unless debugging:

1. Normal: [`mutex_destroy()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/mutex.h#L48)

1. Debug: [`mutex_destroy()`](https://elixir.bootlin.com/linux/v6.13/source/kernel/locking/mutex-debug.c#L112)

---

# Locking: mutex API

[`mutex_lock()`](https://elixir.bootlin.com/linux/v6.13/source/kernel/locking/mutex.c#L256)

1. Uninterruptible: use with caution

[`mutex_trylock()`](https://elixir.bootlin.com/linux/v6.13/source/kernel/locking/mutex.c#L1071)

1. Returns immediately

[`mutex_lock_interruptible()`](https://elixir.bootlin.com/linux/v6.13/source/kernel/locking/mutex.c#L967)

1. Generally suitable for our chardevs

[`mutex_unlock()`](https://elixir.bootlin.com/linux/v6.13/source/kernel/locking/mutex.c#L517)

---

# BEWARE

Where are these locks forbidden?

---

# Non-sleeping locks

Atomic context certain: [`spin_lock()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/spinlock.h#L349) and [`spin_unlock()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/spinlock.h#L389)

Maybe atomic context: [`spin_lock_irqsave()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/spinlock.h#L379)
and [`spin_unlock_irqrestore()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/spinlock.h#L404)

---

# Small atomic values

1. Initialize with
[`atomic{,64}_t` and `ATOMIC_INIT()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/types.h#L178)

1. Read with
[`atomic_read()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/atomic/atomic-instrumented.h#L30) or [`atomic64_read()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/atomic/atomic-instrumented.h#L1608)

1. Set with
[`atomic_set()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/atomic/atomic-instrumented.h#L65) or [`atomic64_set()`](https://elixir.bootlin.com/linux/v6.13/source/include/linux/atomic/atomic-instrumented.h#L1643)

1. Also: `atomic_long_t` and `atomic_ptr_t`

---

![bg fit](/slides/kkey_pics/listen.jpeg)

---

![bg fit](/slides/kkey_pics/simplified.jpeg)

---

![bg fit](/slides/kkey_pics/time.jpeg)

---

![bg fit](/slides/kkey_pics/cache.jpeg)

---

![bg fit](/slides/kkey_pics/readwrite.jpeg)

---

# Racer strategy overview

1. Two processes writing to `kkey`

1. Each note is pressed and released

1. Race condition: a release or press is missing

1. A press without release will linger

---


# Enhanced contention

Ensure real multiprocessing

1. Restrict each process to single CPU

---

# CPU restriction implementation

[`sysconf(3)`, `_SC_NPROCESSORS_ONLN`](https://man7.org/linux/man-pages/man3/sysconf.3.html)

[`CPU_SET, CPU_ZERO`](https://man7.org/linux/man-pages/man3/CPU_SET.3.html)

[`sched_setaffinity(2)`](https://man7.org/linux/man-pages/man2/sched_setaffinity.2.html)

[`D_GNU_SOURCE`](https://man7.org/linux/man-pages/man7/feature_test_macros.7.html)

---

# demo

Can you fix this race?

---

# Summary

`sysfs` can provide a nice API to set/get values in the kernel

---

# Summary

Race conditions in the kernel can be subtle and severe

---

# Summary

Utilize the appropriate lock type for your use case

---

# Summary

Never sleep or block in atomic context!

1. Use spinlocks

---

# End
