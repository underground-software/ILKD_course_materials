## Create a filesystem 🗄️

The objective of this assignment is to build a kernel module that adds support for mounting a new filesystem with a simple on disk representation.

For the purposes of grading, this assignment will be part of the "Programming Assignments" category.

### Outcomes:

* Familiarize yourself with the kernel's VFS and page cache subsystems

* Learn about storing data portably so that it can be accessed by multiple systems regardless of endianness or abi

### What to submit:

* Patch 1 adds `$USER/filesystem/tests/test.c`, your testing program and `$USER/filesystem/tests/Makefile`, which builds your testing program as `test` via the default and `test` targets

* Patch 2 adds `$USER/filesystem/sfs.c` and a new makefile to build `$USER/filesystem/sfs.ko` via the default and `build` targets

* Don't forget a cover letter

* Submit your patches to filesystem@fall2026-uml.kdlp.underground.software

#### Procedure:

0. Rebuild the kernel using the provided seed config

    * This config will add support for a few different new types of block devices:

        * loopback devices `/dev/loopN` that can be used to create a block device whose backing storage is a file

            * These are useful for your testing efforts since you can just create a new file and then attach a loopdev to it

            * You can configure them using the calls to `ioctl` described in `man 4 loop`

        * virtio virtual disk drive that provides a simple mechanism for having persistent storage for the QEMU vm

            * You can create a file of an appropriate size inside your container e.g. `fallocate -l 1MiB disk.img` and then pass `-global virtio-mmio.force-legacy=false -blockdev driver=file,node-name=disk,filename=disk.img -device virtio-blk-device,drive=disk,bus=virtio-mmio-bus.0` on the qemu cmdline to map that file into the vm as a virtual hard drive

            * It will show up as `/dev/vda` within the virtual machine

            * Any data in the file can be read from the block device and any data written will be written back to the file outside of the vm so it persists across reboot

            * This is helpful for manual experimentation:

                * You can examine or alter the state of the filesystem on the virtual disk from outside of the vm with `sfsutils`

                * You can use qemu user-mode emulation on `sfsutils` and pass the path to the backing file from the host container while the vm is shutdown

                * if you need to reboot to try changing your filesystem module you won't lose the state of the data on the disk and can quickly pick up where you left off

    * It also adds support for the ext2 filesystem

        * This is necessary to select some other kernel configs that are needed for writing a filesystem module

        * These configs are not directly selectable and out of tree modules cannot modify the kernel config

0. Write a testing program

    * As with all other testing code in this class, use TAP format, and a similar makefile

    * Write at least 20 tests that the filesystem can pass or fail

    * Verify the behavior of possible error conditions as well as valid operations

    * You can use the provided `sfsutils` utils as an oracle to verify the manipulations of the disk performed by the module are correct

        * Start with two identical SimpleFS disk images

        * Perform the same operation(s) (e.g. making a new file) on each:

        * On one using `sfsutils` (e.g. `sfsutils creat ...`)

        * And on the other by mounting the filesystem and performing syscalls (e.g. `open("/mnt/...", O_CREAT ...`)

        * Unmount the filesystem to flush all the data to disk and then compare the two disk images

        * If the module is working correctly the two disk images should still be identical

0. Implement a module to add support for SimpleFS to the linux kernel

    * The SimpleFS filesystem uses a basic fixed data layout decided at filesystem creation time

        * The space is broken into chunks of a fixed size (that dictates the maximum file size)

        * There are a fixed number of inodes, corresponding to the number of chunks that fit into the total size of the disk

        * Each inode gets its backing storage from the region of the disk corresponding to the chunk whose number matches its number

    * However, metadata about the inodes must be stored somewhere:

        * Chunks from the beginning of the disk are set aside to hold a table of inodes instead of file contents

        * The inode numbers corresponding to those chunks are set aside for use as directories

        * This works because directories do not require disk space

            * The information about file names and the hierarchy of parent directories is stored in the global inode table

        * The inode 0 is a further special case:

            * It represents the root of the filesystem and has no storage on disk, not even an inode structure

            * The bytes that would correspond to index 0 of the inode table are instead the superblock that identifies the file system and contains important metadata

            * The rest of the chunk of the disk corresponding to inode zero acts as the first chunk that can be used for the inode table

    * Structures for the SimpleFS on disk data types are already part of the source code for the provided sfsutils program in `sfs.h`

        * You can include that file directly into your kernel code as well as it only uses linux kernel headers

        * The special types are there to remind you that when reading from / writing to disk you need to be aware of the exact sizes of the types and the byte order (endianness) for multibyte values

        * The `__le64` type represents an unsigned 64 bit integer stored in little endian byte order

            * You can use the kernel functions `__le64_to_cpu` and `__cpu_to_le64` to convert to and from these types and the native `u64` type for the host cpu (i.e. a 64 bit unsigned integer stored in the native host cpu byte order)

            * You should also be careful when casting to / from the `u64` type and other integer types which may vary in size depending on the platform

    * Your module will be called `sfs`

        * Adding support for a new filesystem in linux is an exercise in connecting the relevant existing generic kernel code and data structures for paths, files, and operations on them (VFS), to the on disk data structures specific to your file system

        * Linux will create and manage its own representation of objects from the filesystem in memory as they are accessed and it is your responsibility to provide the appropriate callback functions the kernel can invoke in response to events throughout the lifecycle of these objects to keep the underlying disk representation appropriately in sync

        * It is worth reviewing briefly these kernel objects important to the VFS to get a sense of the bigger picture before diving in to writing code

            * The most important type of object central to a filesystem driver is the kernel's `struct inode`

                * These structures keep track of every file/directory that has been opened and have a one to one relationship with the underlying inodes on the disk

                    * I say "one to one" with precisely the same meaning it has in mathematics (i.e. injective)

                        * For every `struct inode` that the kernel has, there is a corresponding inode with that number on disk within that filesystem

                        * There is never more than one `struct inode` for a given inode number at given time (within the scope of a particular mounted instance of a filesystem)

                    * However the opposite is not necessarily the case (i.e. the mapping is not a surjection)

                        * It is possible that an inode of a given number exists on disk and refers to a real file, but no one has accessed a path that refers to it and so that inode has never been looked up

                            * In that case, there will be no `struct inode` in the inode cache yet

                        * Or it is possible that the kernel will evict `struct inode`s from the cache if it gets too full

                            * The kernel will invoke a callback from the module to write any modifications to the inode struct back to the on disk representation

                            * Then there will again be no such struct inode in the cache (until it is accessed again)

                * The driver frequently has to produce instances of these inode structures for files it creates or looks up

                    * It gets them from a cache that the kernel maintains per filesystem mapping inode numbers to struct inodes

                    * It is the responsibility of the filesystem to initialize any newly created cache entries with the appropriate data from the backing disk inode

            * Each of the inode structures represent a particular filesystem object, but users of filesystems refer to files by path or file descriptor

            * The kernel stores paths inside of `struct dentry` objects

                * It is the responsibility of the kernel's `struct dentry` cache to manage the association between file system paths and the corresponding inodes

                    * (or lack thereof in the case of a nonexistent path - these are also cached for performance)

                * The dentry cache relies on the filesystem driver to fill the corresponding inode structure into the dentry struct for paths that are looked up or created on the filesystem

                * Once a path has been converted into a file descriptor (i.e. by opening it), the struct inode obtained from the dentry for that path within the file system is stored within the `struct file` representing the file description pointed to by the corresponding file descriptor within that process

                * As operations are performed on that file descriptor, the appropriate callbacks are located by consulting the `struct file_operations` from the corresponding inode

                * The filesystem driver is responsible for filling that member with an appropriate table of methods when initializing inodes

            * The final important piece of the puzzle from the VFS for file system drivers is the `struct super_block`

                * The kernel creates one of these structures each time a new mount is attempted and initializes it with information about the backing storage that is being mounted

                    * The filesystem driver can pass the superblock to functions from the page cache subsystem

                    * These functions are able to look at the information stored within the superblock concerning the backing storage and provide access to blocks of the disk

                    * The driver is then able to read or write that data, but like loaning a book from the library, it must also let the page cache know when it is done with the data

                    * Further, it must also let the page cache know if it modified any data so that the modifications will make their way back to the disk eventually

                * During an attempt at mounting, the filesystem must examine the on disk superblock from the backing storage to ensure that it matches what it expects to see

                    * If the filesystem can be mounted, the driver must locate the inode corresponding to the root directory and put it in a new dentry that gets stored into the super block

                    * Whenever path resolution proceeds into the filesystem through its mount point, this dentry and its inode will act as a gateway for accessing the rest of the filesystem

    * The following is a summary of that which is necessary to implement the module

        * Particular attention is paid to listing the existing generic functions from the kernel that are relevant

            * These functions can be looked up in the kernel sources (e.g. on <https://elixir.bootlin.com>)

            * Some of them have doc comments that describe their behavior

            * All of them are used in other filesystem drivers which provide real world examples of their use

        * I would recommend reading the following bullet points in a sort of "breadth first" order visiting each of the top level bullets before diving into the next layer and so on, sketching out all of the driver functions but not finishing any of them at first, just leaving yourself `//FIXME:` comments with the big picture steps

            * Then once you have a sense of the bigger picture you can go back and start filling in the details

        * I would expect to need to use a debugger to figure out what is going on with your code

            * Now that we have an mmu it is easier to attach gdb to the kernel

            * You can pass the `-s` flag to qemu to have it set up its socket for gdb

                * You can just always pass this option if you omit the `-S` option that stops the system on startup

                * When you want to debug you will already have the listener ready so you just need to attach gdb

            * In a second terminal you can `cd` into your linux kernel folder and run `gdb vmlinux`

            * You can then attach to qemu with `target remote localhost:1234`

                * This will freeze the vm until you run `continue`, but you might want to set up some relevant breakpoints first

            * In order to actually debug your module, you need to tell gdb about it, and where the kernel has mapped it into memory

                * In the vm terminal, you can use `cat /proc/modules` to dump the list of loaded modules and where they live in RAM

                * In gdb, run `add-symbol-file /path/to/your/module.ko -o 0xVIRTUAL_ADDRESS_FROM_PROC_MODULES` filling in the appropriate path and memory location

                * You will then be able to put breakpoints on functions from your module

    * The module will need to register a new file system using `register_filesystem` on startup

        * During cleanup the filesystem must be unregistered using `unregister_filesystem`

        * These functions take a `struct file_system_type` which describes the nature of the filesystem, its requirements, and how to mount it

        * This structure has many fields but only the following need to be initialized:

            * `owner`: associate the file system with `THIS_MODULE` so that the kernel can keep track of who owns it and prevent the module from being removed while the filesystem is in use

            * `name`: identify the file system with the unique identifier `"sfs"` that can be passed as the `type` to the mount syscall to select this filesystem

            * `fs_flags`: indicate that a backing block device must be provided with `FS_REQUIRES_DEV`

            * `kill_sb`: callback function pointer for unmounting

                * The generic `kill_block_super` function from the kernel is sufficient

            * `init_fs_context`: callback function pointer for preparing to mount the filesystem

                * This function will be given a `struct fs_context *` and will need to initialize its `ops` member with a pointer to a `struct fs_context_operations` for the driver

                * This `fs_context_operations` needs only to have its `get_tree` member specified

                * A small wrapper around the generic `get_tree_bdev` function from the kernel is sufficient

                    * The filesystem context argument can be forwarded to the kernel helper

                    * However it requires an additional argument: a callback to fill the superblock

    * The fill superblock callback has several important responsibilities:

        * Verify that the data on the backing storage actually looks like a SimpleFS filesystem by reading and examining the on disk superblock:

            * Access the very first bytes of the disk by requesting sector 0 from `sb_bread`

                * The actual data will be found within `b_data` in the returned `struct buffer_head`

                * Be sure to call `brelse` to unpin the data from the page cache once you have copied out the important data

            * Verify that the expected magic string is present

            * Ensure that the file system metadata (e.g. number of inodes, max file size, etc.) is sane (i.e. self consistent, and within the bounds of the actual size of the block device as reported by `bdev_nr_sectors`)

            * If these checks fail, the mount shall be aborted by returning a negative error of `EINVAL`

        * Initialize important fields of the provided `struct super_block *`:

            * `s_maxbytes` indicate the max file size for this mount based on the metadata from the superblock so that the kernel enforces those bounds automatically

            * `s_blocksize` (via `sb_set_blocksize` to perform validation and keep s_blocksize_bits in sync) a fixed value of `SECTOR_SIZE` aka `512` will make things simple

            * `s_fs_info` with a pointer to a private structure for simplefs data

                * Dynamically allocate an instance of this structure since it holds data specific to this mount

                * Store important metadata from the superblock

                * Include a lock to regulate access to the shared inode table from concurrent operations on the filesystem

                    * The kernel's `struct rw_semaphore` is a good choice because some operations only need to read the inode table and so multiple of them may proceed simultaneously

            * `s_op` with a pointer to the module's `struct super_operations` - only two members are needed

                * `put_super` that cleans up the memory allocated for `s_fs_info`

                * `write_inode` that updates the on disk representation of an inode with any changes made to the kernel `struct inode *`

                    * The only possible change that is relevant is an update to the file size

                    * Acquire appropriate exclusive access to the inode table with the locks from the private structure (all `struct inode`s have a pointer to their super block in `i_sb`)

                    * Obtain a pointer to the corresponding disk inode at the appropriate offset within the appropriate sector based on the inode number using sb_bread

                        * This is something you will be doing often, it probably makes sense to write a helper for it, but be sure to give the caller access to the `struct buffer_head` as well as the actual pointer to the data they requested so they can release / dirty it

                    * Check the file size from the inode with `i_size_read`

                    * Update the size on disk if needed being sure to handle the endianness conversion appropriately

                        * Mark the buffer dirty with `mark_buffer_dirty` after making changes to the data from the buffer head

                    * The `struct writeback_control *` can indicate that this request needs to have the changes synced to disk immediately

                        * This occurs if a user explicitly syncs a file to disk

                        * The kernel is able to sync the file data using a different mechanism but needs the help of the fs driver to sync the inode metadata

                        * If the `sync_mode` is `WB_SYNC_ALL` explicitly call `sync_dirty_buffer` on the `buffer_head`

                        * Ensure that there was no error in syncing: forward the returned error code from `sync_dirty_buffer` if it is nonzero

        * Create the root directory entry `struct dentry` and associate it with the `struct inode` for inode `0` the root directory

            * Obtain the inode for the root directory from the kernel inode cache

                * This operation of looking up a struct inode for a given inode number will be common enough to warrant a dedicated helper

                * `iget_locked` will return a `struct inode *` from the kernel's inode cache for the super block given an inode number

                * If the inode was already found in the cache the inode's state will not have the `I_NEW` bit set and the structure can be returned without further modification

                * If the inode is new, it will need to be initialized:

                    * `i_ino` needs to be set to the inode number

                    * `i_op` needs to be set to the module's `struct inode_operations` table

                    * `i_mapping->a_ops` needs to be set to the module's `struct address_space_operations` table

                    * `i_fop` needs to be set to one of the module's `struct file_operations` table

                        * Two different tables are needed, one for regular files and one for directories

                    * `i_mode` needs to be set to an appropriate value based on whether the inode refers to a file or a directory

                        * SimpleFS does not track file ownership or enforce permissions, all files can be given the base mode of `0777`

                        * Regular files should have `S_IFREG` ORed into the mode while directories should have `S_IFDIR`

                    * `i_size` needs to be filled in with the file size from the on disk inode if the inode refers to a regular file

                        * Modifications to `i_size` must not be performed directly, instead `i_size_write` must be used for synchronization purposes

                        * Appropriate locking as always must guard access to the on disk inode table

                        * Endianness must be accounted for whenever data is read off of the disk

                    * The `I_NEW` state must be cleared from the inode before it is returned

                        * `unlock_new_inode` handles this

            * Make a root `struct dentry` for the returned inode

                * `d_make_root` creates such a dentry

                * Be sure to check for failure (`NULL` return indicates allocation failure)

                * Assign the new `dentry` to `s_root` in the superblock

    * The module's `struct inode_operations` table provides callbacks for interacting with filesystem metadata (creation/removal/discover of files)

        * `lookup` is responsible for maintaining the kernel dentry cache by looking up files within the filesystem

            * It has to walk the inode table to find whether an inode exists whose name and parent directory match the requested information

            * If such an inode exists, the `struct inode` for that number has to be located (you have a helper for this)

            * The `d_splice_alias` function is used to connect the inode to the dentry

                * If there was no match, this function should still be called to cache the failed lookup but a `NULL` pointer should be passed for the inode

                * The return value of this function should be returned from lookup as is

        * `create` is responsible for making new regular files

            * It has to walk the inode table (starting from the first inode number not reserved for directories) looking for an unused inode

            * Once an inode whose name field is empty has been located (i.e. an unused inode), that inode can be initialized with the information about the new file

            * The name can be copied in and null padded if it is less than 16 bytes in length

                * Make sure to account for the possibility that the provided name is too long and return an appropriate error

            * The parent directory can be set (making sure to account for endianness)

            * The file size can be set to zero

            * The `struct inode` for the new inode number must be looked up and associated with the dentry

                * You have a helper for finding the inode, and then you can call `d_instantiate` to associate it with the dentry

        * `mkdir` is exactly the same as create except that it walks the region of the inode table for directories looking for an empty inode

            * The callback returns a `struct dentry`, `NULL` is actually the expected value if the call succeeds

            * A valid nonnull value is only used if the module creates a new dentry instead of reusing the existing one (our driver will never do this)

            * An error is signaled using the `ERR_PTR` function to convert a negative errno into a special invalid pointer value

        * `unlink` removes a regular file

            * The entry in the inode table for the files inode can simply be `memset` to zero to mark it as unused

            * Make sure the link count on the kernel's in memory `struct inode *` goes to zero so it is evicted as well

        * `rmdir` is very similar to `unlink` but for directories, however:

            * You must verify that a directory has no children before it can be deleted

                * The full inode table must be walked to look for any inodes that list the directory to be deleted as their parent

                * If there are children, the `-ENOTEMPTY` error shall be returned

            * If no children are found the same steps can be taken as in `unlink` to actually complete the operation

    * The module's `struct file_operations` tables specify callbacks for performing syscalls to access and manipulate files in the file system

        * For regular files, generic kernel implementations can be provided that rely on the `struct address_space_operations`

            * `read_iter`: `generic_file_read_iter`

            * `write_iter`: `generic_file_write_iter`

            * `llseek`: `generic_file_llseek`

            * `mmap_prepare`: `generic_file_mmap_prepare`

            * `fsync`: `simple_fsync`

        * For directories:

            * `read` can be specified as `generic_read_dir` that will return `-EISDIR` to ensure calls to `read` fail

            * `fsync` can be specified as `noop_fsync` as directories do not have any storage other than their inode

            * `iterate_shared` is how directory contents are listed

                * The callback emits directory entries as it finds them, but must be able to cope with stopping part way through once the receiving buffer fills up and resuming

                * The callback receives a `struct dir_context` and can use the `pos` member to track its progress as it walks through the inode table

                * The pos is opaque to the caller and the driver can use it freely, it can simply translate directly into the current iteration of the loop within the inode table for ease of resuming

                * Each time an entry is found `dir_emit` is called to submit it to the caller

                    * The return value indicates whether or not the loop should continue

                    * Once `dir_emit` returns `false` no more entries can be submitted until the buffer is emptied by the caller and the callback should return

                    * If more entries are required, the callback will be invoked again with the same context as before and it can continue where it left off

                * An entry for `.` and `..` must be emitted as well

                    * There are special helpers `dir_emit_dot` and `dir_emit_dotdot` just for this purpose

                    * Be sure to emit these only once - you can emit them before and/or after the main loop over all the inodes in the table, but you will need to decide what value of ctx->pos corresponds to emitting one of these special entries vs starting/resuming the loop

    * The module's `struct address_space_operations` table provides an interface for identifying which blocks on the disk correspond to a particular sector within a particular inode

        * Most members are either generic kernel implementations or minimal wrappers around generic functions:

        * `dirty_folio`: `block_dirty_folio` is sufficient

        * `invalidate_folio`: `block_invalidate_folio` is sufficient

        * `write_end`: `generic_write_end` is sufficient

        * `read_folio`: a small wrapper around `block_read_full_folio` is needed that supplies a callback to locate a file block specific to the filesystem

        * `writepages`: a small wrapper around `mpage_writepages` is needed that likewise supplies the same callback for locating file blocks

        * `write_begin`: a wrapper around `cont_write_begin` is needed that supplies the file block callback

            * The `cont_write_begin` function is itself a wrapper around `block_write_begin` for filesystems which do not allow there to be "holes" (regions of unallocated data) in a file

                * Our filesystem does not allow holes because it does not dynamically allocate blocks and instead uses a fixed mapping from inode number to region of the disk

                * If the user wishes to start writing at a position beyond the end of the file, `cont_write_begin` will take care of zeroing the data in between the previous end point and the new start

                * However, to do that it needs to know the current size of the file (as obtained from `i_size_read` on inode pointer from the mapping's `host`) 

            * This function must also handle some error clean up if the `cont_write_begin` returns a nonzero value

                * Compare the end position of the desired write (starting position plus length) and the size of the inode (as obtained from `i_size_read` on inode pointer from the mapping's `host`):

                * If the desired end position exceeds the actual size, call `truncate_pagecache` with the correct size from the inode

                * This will remove any entries in the page cache that were preemptively created based on the expected size of the write but that were not actually written to

        * The file block callback has to calculate the sector number on the disk corresponding to a given sector offset within a particular file

            * The inode number multiplied by the max file size indicates the start of the region for that file (but do not forget we are working in 512 byte sectors here)

            * That value can be added to the desired sector offset within the file to get the overall disk sector for that file region

            * The `map_bh` function is used to provide a mapping for the `buffer_head` based on the disk sector

            * If the mapping is from a write call, new file data may be being created (a non zero value will be passed for the final integer argument to the callback):

                * If new data is being created, we need to call `set_buffer_new` on the `buffer_head` 

                * Since our filesystem does not support holes within a file, the only time new data is being created is when a file is having more data added at the end

                * Look at the size of the file (from `i_size_read`) and the block being requested to determine whether this is new data is being appended and only call `set_buffer_new` if it is

0. Extra credit opportunities

    * The module as described does not support renaming files (e.g. if you try to use `mv` on a file within an sfs mount it will fail)

        * Add support for renaming to the driver

        * Add a new command to sfsutils to rename files

    * The `rmdir` operation is slightly suboptimal because it needs to scan the entire inode table to see if there are any children before it can proceed

        * The `file_size` member of inodes referring to directories is currently unused and could be put to work

        * Enable faster `rmdir` performance by using the `file_size` member of each directory inode to track the number of children it has

            * This means keeping it in sync every time a file is created or removed, but then `rmdir` can just check if it is zero

            * Be wary of the special semantics of inode 0 - the root directory can never be deleted anyway, but if you are not careful a naive implementation of this optimization might trash your superblock

            * This change should be backwards compatible with the existing version of the driver as it will ignore the sizes of the directories and rely on the slower scan of the table

        * Make the appropriate changes to your driver and `sfsutils` to enable this optimization

            * Include a new command in sfsutils to migrate a disk image created / modified without this optimization to the new format by filling in the sizes based on the number of children for each directory

    * The disk layout described for SimpleFS is very simple while still being pretty optimized, however I am aware of at least one general optimization that is theoretically possible:

        * The amount of inodes set aside for directories might be larger than the number of blocks required to hold an inode table of the necessary size

            * (i.e. while the size of the inode table sets a lower bound on the number of inodes that must be set aside for directories, the actual number the user wishes to select might be larger)

            * If this is the case, the on disk blocks that would be the backing storage for these directory inodes is simply completely wasted, used neither for the inode table nor for file contents

        * This situation could be improved by reusing those blocks to be the backing storage for a few more inodes

            * Though care would need to be taken when expanding the inode table with the entries for these files to not spill into one of the blocks this very optimization is trying to utilize

            * To be semi-backwards compatible and allow migration, these blocks could be used for inode numbers past the previous limit (instead of shifting the allocation scheme for all of the inodes)

                * If the superblock metadata is not modified and the driver just knows to look past the stated end of the inode table the disk image of a filesystem using this optimization could be mounted on a version of the module without this optimization and the entries would simply be invisible

                * To ensure that these entries would not interact poorly with rmdir (i.e. parent dir is unaware of these and gets deleted even though they exist) these inodes could be reserved exclusively for files created in the root directory (which is unable to be removed)

        * Make the appropriate changes to your driver and the sfsutils program to implement this optimization

            * Include a new command in `sfsutils` to migrate a disk image created without this optimization to the new format by initializing the new inode table entries as empty

    * The SimpleFS filesystem does not support symlinks or device special files (i.e. `symlink(2)` or `mknod(2)`)

        * Support for them could be introduced at the expense of backwards compatibility using the following scheme:

            * Filenames cannot contain `/` so it could be used in the name field as a marker for these special types of files

                * It could be placed at the beginning to make it easy to check for - special types of files would have a limit on the size of their name one shorter than all other sorts, but this is acceptable

                * Symlinks are similar to regular files in that they are relatively common and have contents of a variable length

                    * As such, symlink inodes can be allocated from the regular file inode pool and use the backing storage for their inode to store their target

                * Other special files from mknod (fifos, char/block devices, & sockets) are similar to directories in that they are relatively uncommon and do not need on disk storage beyond their inode

                    * As such, special file inodes can be allocated from the directory inode pool

                    * The `file_size` member of the inode can be reused to store the necessary information

                        * The `dev_t` holding major/minor number for device special files is a `u32` so it can go in the lower part of the size while the mode is only a `u16` so the special part of it (i.e. `S_IFIFO` `S_IFSOCK` `S_IFCHR` `S_IFBLK`) can go in the upper 32 bits

        * Make the appropriate changes to your driver and the sfsutils program to add support for these special files

            * Include new commands in `sfsutils` to create them

    * If you choose to complete some or all of these ideas for extra credit, first make an extra commit where you just create a copy of the sfsutils code within your folder for this assignment

        * Then, for each of the extra credit ideas you completed, create a commit that modifies your driver and your copy of sfsutils as needed and adds at least five new tests to the testing program to verify the new behavior

        * In other words, if you completed only the first extra credit suggestion, your patch series would have 4 patches (5 including the cover letter), if you completed all four your patch series would have 7 patches (8 including the cover letter)

[Frequently Asked Questions](/faq.md)
