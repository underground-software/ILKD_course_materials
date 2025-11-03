## Create a block device 🧱

The objective of this assignment is to build a kernel module that creates simple block devices backed by RAM.

For the purposes of grading, this assignment will be part of the "Programming Assignments" category.

### Outcomes:

* Familiarize yourself with the kernel's block subsystem and block io `bio` interface

* Learn about module parameters for load time configuration of kernel modules

### What to submit:

* Patch 1 adds `$USER/block_device/spec.txt`, a filled out copy of `spec.txt`

* Patch 2 adds `$USER/block_device/tests/test.c`, your original testing program, and `$USER/block_device/tests/Makefile`, which builds your testing program as `test` via the default and `test` targets

* Patch 3 adds `$USER/block_device/memblk.c` and a new makefile to build `$USER/block_device/memblk.ko` via the default and `build` targets

* Patch 4 adds `$USER/block_device/questions.txt`, a filled out copy of `questions.txt` and `$USER/block_device/testing_output.txt` which shows successful manual and automated testing of the module

* Don't forget a cover letter

* Submit your patches to block_device@fall2025-uml.kdlp.underground.software

#### Procedure:

0. Rebuild the kernel with support for devtmpfs filesystem for automatically created device nodes and the block layer

    * Identify the appropriate config options to enable and enable them using `menuconfig`

        * Answer the relevant question in questions.txt

    * Rebuild the kernel image and module dependency/symbol information with your new config

0. Edit the rootfs to automatically mount the devtmpfs on /dev

   * Make the appropriate edits to files/folders in rootfs to automatically mount the kernel's devtmpfs on /dev during boot

       * Answer the relevant question in questions.txt

0. Rebuild busybox with support for the fdisk partitioning utility

   * Identify the appropriate config options to enable the fdisk utility and allow it to write partition tables

       * Answer the relevant question in questions.txt

   * Enable the options using `menuconfig` and rebuild busybox

0. Write your specification for the module

    * Copy the template into your folder and fill it out

    * You can now make your first commit

0. Write a testing program (and makefile) that verifies that the module complies with the spec

    * The makefile should behave the same as the one specified for the testing program in first_module

    * The testing program output must follow the TAP specification like the testing program from first_module

    * The testing program should not only perform unit tests of the behavior of the block device once it is loaded

    * The testing program should also perform end to end integration tests of loading and unloading the module

        * Be sure to test the behavior of the module parameters including edge cases like invalid or missing parameters/values

        * Be sure to test the behavior of partitioning the disk using fdisk

    * You should have at least 20 distinct tests that the module can pass or fail

    * You can now make your second commit

0. Implement the module

    * Your module will be called `memblk`

    * It will have two parameters `memblk_num_devices` and `memblk_size` to configure how many devices to create and how big in kibibytes they should be

        * A value of zero doesn't make sense for either parameter, so you can initialize the variables with zero and check whether they were set during the init function

        * The if you set an appropriate type for the parameter, the generic kernel code for parameters can handle rejecting invalid values

    * The driver will need to dynamically allocate a block device major number using `register_blkdev`

    * For each of the requested number of devices, the driver will need to setup a `struct gendisk`:

        * An instance of the struct for you to customize can be obtained from `blk_alloc_disk`

            * You can pass null for the queue limits to use default settings

            * You can pass `NUMA_NO_NODE` to indicate that this device does not have an affinity for any particular CPU

        * The disks will all share the dynamically allocated major number

        * Each disk will get its own range of minor numbers

            * Allow for maximum flexibility by giving each device `DISK_MAX_PARTS` minors

            * Set the `first_minor` value appropriately so that the range of possible minor numbers of distinct devices do not overlap

        * The devices should use `disk_name`s `memblk0`, `memblk1`, etc in sequence

        * The `private_data` can be used to store a pointer to the backing RAM buffer

            * Allocate an appropriately sized backing RAM buffer based on the value of `memblk_size` from zero initialized memory from the kernel allocator

            * Be sure to use `set_capacity` to indicate the size of the disk as well

                * Linux always operates with `512` byte logical sectors, perform the appropriate conversion from raw size into sectors

        * The `fops` member will need to point to a dedicated `struct block_device_operations` specific to the module

        * The `struct gendisk` for each device will need to be published after being initialized using `add_disk`

    * Within the `struct block_device_operations` for the driver:

        * Specify `THIS_MODULE` for the `owner`

        * The only method you need to implement is `submit_bio`

            * This method is called each time an io request needs to be fulfilled by the driver

    * Within the callback function for `submit_bio`:

        * The passed pointer to a `struct bio` describes the request that needs completing

            * The type of request (only reading or writing for this simple driver) is determined by the `bi_opf` member

            * The specific device the request wishes to act on is given by the `bi_bdev` member

            * The actual request consists of a series of `struct bio_vecs` that request copying a certain amount of data to or from a particular offset within a specific page of memory

            * Access to the `struct bio_vecs` that make up the request is granted by a `struct bvec_iter` member `bi_iter` which can be used to iterate over each bvec sparing your driver the gory details of how and where they are stored

        * You can find the struct gendisk corresponding to the given disk in `->bi_bdev->bd_disk` of the `struct bio`

            * This will let you find the appropriate backing RAM buffer because you stored it in the gendisk's `private_data` field

        * Use `for_each_bvec` to iterate over each `struct bio_vec` within the `struct bio`'s `struct bvec_iter bi_iter` member

            * Each `bvec` corresponds to a request to copy up to a single page worth of memory

            * The appropriate offset within the backing memory store can be calculated using the `bi_sector` member of the `bvec_iter` (recall that linux always uses 512 byte logical sectors)

            * Whether data is being read or written can be determined by calling `op_is_write` on the `bi_opf` member of the `struct bio`

            * The actual copy can be done using the helpful `memcpy_{to,from}_bvec` functions which handle setting up an appropriate mapping temporary to access the page and copying the data

        * Don't forget to call `bio_endio` on the `struct bio` to mark it as complete before returning from your callback

    * Make sure to properly clean up all resources when unloading the module *and* if any error occurs during setup (i.e. properly unwind anything that you already succeeded at before encountering the error)

        * Memory from the kernel allocator must be `kfree`'d

        * A struct gendisk is allocated in two steps, `blk_alloc_disk` and `add_disk`

            * The opposite of `blk_alloc_disk` is `put_disk`

            * The opposite of `add_disk` is `del_gendisk`

            * Make sure to unwind the functions in the appropriate order and only undo actions that succeeded

0. Test your work and document your findings

    * Rebuild the rootfs

        * Make sure you copied in your testing program and your module

        * Make sure you included the updated the busybox binary

    * Boot into the new kernel capturing output into the `testing_output.txt` file

        * Load the module and perform some manual testing using cmdline utilities

        * Unload the module

        * Run the testing program letting it complete its suite of tests

0. Extra Credit Opportunity

    * Add code to the kernel to recognize a new partition table format with the following basic structure:

        * First 512 byte logical sector identifies partition table and stores the number and sizes of partitions

        * First 7 bytes are the magic string `KDLPART` 8th byte `n` indicates how many partitions there are (up to 63)

            * The following `n` groups of 8 bytes are a simple array of `sector_t` values storing the sizes of each partition in sectors

        * The subsequent sectors are for the partitions

            * There are never any gaps between partitions

    * New partition tables can only be added as builtin drivers, not as modules

        * You will need to modify the kernel source and submit a patch containing your modifications within a fifth patch

        * Add a config entry in `block/partitions/Kconfig` to control whether your partition table code is built

        * Edit the corresponding makefile appropriately to conditionally compile a new file `block/partitions/kdlp.c` based on the config option

        * Add a function pointer to your check function to the array in `core.c`

        * In your callback, check for the magic string, and if found, parse and verify the table then call `put_partition` for each partition you find

    * In a sixth patch:

        * Include the source code for a simple cmdline utility written in C to create one of these tables on a blockdevice and make the kernel reload the partitions

        * Demonstrate running it in the VM with your patched kernel and making partitions on your memblk driver

[Frequently Asked Questions](/faq.md)
