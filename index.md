## 📝 ACTC Assignment 2 : Build a "Mini" Container Runtime

This assignment will be formatted and submitted using a simulated Linux kernel mailing list patchset submission and review workflow.

Details of the assignment itself are presented first, followed by submission instructions.

Before begining work for this assignment, please obtain your credentials by entering your student ID on the [registration](https://winter2025-iit.actc.underground.software) page.

Then, please enter our fedora container with the following invocation, setting `$username` and `$password` appropriately beforehand.

```bash
sh -c 'read -rp "username: " username && curl -u $username https://winter2025-iit.actc.underground.software/Containerfile | podman build -t kdlp_container -'
```

Why do this, why skip... etc TODO

**Due dates**

|Component|Date|
|Initial Submission Deadline|EOD Thursday, 25 December 2025|
|Peer Review Deadline|EOD Sunday, 28 December 2025|
|Final Submission Deadline|EOD Tuesday, 30 December 2025|

*All due dates are 23:59 IST on the day stated*


### 1\. Overview

In this assignment, you will demystify container technology (like **Docker** or **Podman**) by writing a simple container runtime in C from scratch. You will interact directly with the **Linux kernel's isolation primitives**—**Namespaces**, **Cgroups**, and **Seccomp**—to create a secure, isolated environment for a process.

By the end of this task, you will have a program that can run a shell inside a custom root filesystem, isolated from the host's process list, hostname, and protected by security policies.

-----

### 2\. Prerequisites

  * **OS**: Linux (Virtual Machine recommended).
  * **Permissions**: Root access (`sudo`) is required for namespace operations.
  * **Packages**: You will need the development headers for capabilities and seccomp:
    ```bash
    sudo dnf install gcc make libcap-ng-devel libseccomp-devel
    # sudo apt-get install gcc make libcap-ng-dev libseccomp-dev
    ```
  * **Knowledge**: Basic C, Linux system calls (`fork`, `exec`, `mount`), and CLI usage.
  * **Resources: Root Filesystem (rootfs)**: You can export one from Alpine Linux or Fedora Linux using podman:
    ```bash
    [sudo] mkdir rootfs
    [sudo] podman export $(podman create alpine) | tar -C rootfs -xvf -
    ```

-----

### 3\. Program Requirements

Your program must be named `simple_container` and accept the following arguments:

```bash
sudo ./simple_container <rootfs_path> <command> [args...]
```

  * `<rootfs_path>`: The directory to become the new root (e.g., `./rootfs`).
  * `<command>`: The binary to execute inside the container (e.g., `/bin/sh`).

#### Core Rules

  * **Strict Error Handling**: Every system call (e.g., `mount`, `unshare`) must be checked. If it fails, print a descriptive error message and exit.
  * **Comments**: Explain why you are making specific calls (e.g., "Remounting / as private to prevent mount leakage").

-----

### 4\. Implementation Steps

#### Phase 1: The Skeleton & Namespace Isolation

Create the basic process structure. Container runtimes typically use `clone` or `unshare` to create new namespaces.

  * **Goal**: The child process should run in a separate **UTS (Hostname) namespace**.

###### Tasks:

1.  Use `fork()` to create a child process.
2.  The parent waits for the child to exit.
3.  Child uses `unshare(CLONE_NEWUTS)` to detach its hostname from the host.
4.  Child changes its hostname to `mycontainer`.

<!-- end list -->

  * **Verify**: Running `hostname` inside the container shows the new name, while the host remains unchanged.

-----

#### Phase 2: Filesystem Isolation (The Jail)

Now, trap the process inside the provided `rootfs` directory. **Do not use chroot**. You must use **pivot\_root** for better security.

  * **Goal**: The process sees `<rootfs_path>` as `/`. The old host filesystem is inaccessible.

###### Tasks:

1.  **Private Mounts**: Mark the root mount (`/`) as `MS_PRIVATE` so mount events don't leak to the host.
2.  **Bind Mount**: `pivot_root` requires the new root to be a mount point. Bind mount the `rootfs` path to itself.
3.  **Pivot**: Use `syscall(SYS_pivot_root, ...)` to swap the root.
4.  **Cleanup**: Unmount the old host root (detach it) and remove the temporary directory.

<!-- end list -->

  * **Verify**: Running `ls /` inside the container shows only the Alpine filesystem.

-----

#### Phase 3: PID Isolation (Process Identity)

Isolate the process IDs so the container cannot see host processes.

  * **Goal**: The command running inside the container should be **PID 1**. Running `ps` should show only container processes.
  * **Concept**: `unshare(CLONE_NEWPID)` affects the children of the calling process, not the process itself. To make our child PID 1, we must set up the namespace **before forking**.

###### Tasks:

1.  **Refactor Unshare**: Move your `unshare(CLONE_NEWPID)` call from the Child process to the **Parent** process (before the `fork`).
2.  **Single Fork**: You do not need to fork twice. The existing `fork` from Phase 1 is sufficient.
3.  **Mount /proc**: The `ps` command relies on the `/proc` filesystem. You must mount a fresh `proc` filesystem at `/proc` **after** pivoting root.

<!-- end list -->

  * **Verify**: Run `ps aux` inside the container. You should see very few processes, and your shell should be PID 1.

-----

#### Phase 4: Resource Isolation (Cgroups)

Prevent the container from consuming all system memory using **Cgroup v2**.

  * **Goal**: Limit the container to **100MB** of RAM.

###### Tasks (in Parent Process):

1.  **Create Group**: Create a directory `/sys/fs/cgroup/simple_container`.
2.  **Set Limit**: Write `"100000000"` (100MB) to `memory.max` in that directory.
3.  **Add Process**: Write the Child's PID to `cgroup.procs`.
4.  **Cleanup**: After the child exits, remove the cgroup directory using `rmdir`.

-----

#### Phase 5: Security - Capabilities

The root user inside a container shouldn't be as powerful as the real root.

  * **Goal**: Drop dangerous capabilities (like rebooting, loading kernel modules).
  * **Library**: Use **libcap-ng**.

###### Tasks (in Child Process):

1.  Create a **whitelist** of allowed capabilities (e.g., `CAP_CHOWN`, `CAP_KILL`, `CAP_NET_BIND_SERVICE`).
2.  Use `capng_clear(CAPNG_SELECT_BOTH)` to wipe the slate clean.
3.  Use `capng_update` to add your whitelist to **Effective**, **Permitted** and **Bounding** sets.
4.  Apply the changes with `capng_apply`.

-----

#### Phase 6: Security - Seccomp (Syscall Filtering)

Restrict which system calls the container can make to the kernel.

  * **Goal**: Prevent the container from calling `reboot()`, `swapon()`, or kernel module functions.
  * **Library**: Use **libseccomp**.

###### Tasks (in Child Process):

1.  Initialize a filter with `seccomp_init(SCMP_ACT_ALLOW)` (**Allow-list** by default).
2.  Add rules to block specific syscalls: `reboot`, `swapon`, `swapoff`, `init_module`, etc. Use `SCMP_ACT_ERRNO(EPERM)` to return a permission error.
3.  Load the filter into the kernel using `seccomp_load`.

-----

### 5\. Helpful Snippets

`pivot_root` wrapper is not in the standard libc headers. You must define a wrapper:

```c
static int pivot_root(const char *new_root, const char *put_old) {
    return syscall(SYS_pivot_root, new_root, put_old);
}
```

**Compiling**: You must link against the security libraries:

```bash
gcc -o simple_container simple_container.c -lcap-ng -lseccomp
```

-----

### 6\. Submission


Submit a directory/zipfile/tarfile/patches-by-mail with a single C file named **simple\_container.c**, a **README** and **Makefile**.

* Patch 1 adds your makefile as to the file `<username>/crun/Makefile`

* Patch 2 implements phase 1 by creating `<username>/crun/simple_container.c`

* Patch 3 implements phase 2 by modifying `<username>/crun/simple_continer.c`

* Patch 4 implements phase 3 by modifying `<username>/crun/simple_continer.c`

* Patch 5 implements phase 4 by modifying `<username>/crun/simple_continer.c`

* Patch 6 implements phase 5 by modifying `<username>/crun/simple_continer.c`

* Patch 7 implements phase 6 by modifying `<username>/crun/simple_continer.c`

* Don't forget a cover letter containing what your would put in the README

* Submit your patches to `runtime@winter2025-iit.actc.underground.software`

##### Submitting this assignment

The assignment must be submitted in the form of an email patchset
generated by `git format-patch` from commits made in your local copy of
[this repository](https://winter2025-iit.actc.underground.software/cgit/ILKD_Submissions/)
that includes a cover letter descriping your work. You will use `git send-email` to submit the assignment as described above.

As part of the peer review process,
this assignment will require you to submit your patchset at least twice.

Start the assignment early. If you run into issues and get stuck it gives
you time to ask questions and get help before the deadlines
so you can submit something on time and get credit for the assignment.
If you finish early, you can resubmit as many times as you'd like.

Any submission that violates these guidelines or fails to compile with no warnings or errors will receive a zero.

With the exception of presentations, all work in this course takes place on our mailing list.
Students submit assignments and review peer submissions on this list.

Each assignment involves the following three stages:

**Step 1: Initial Submission (due _Thursday, 25 December 2025_)**

* The student makes their submission to the mailing list using `git send-email`

* If the initial submission is late, the student will get a zero on the entire assignment

* The subject line of the initial submission patchset should be tagged as a "Request for Comments", otherwise known as RFC, [explained here](patchsets.md)

* Each [re-submission](resubmitting.md) should increment the version number in the subject line, [explained here](patchsets.md)

**Step 2: Peer Review (due _Sunday, 28 December 2025_)**

* Each student is assigned two other students' work to review on their [submission dashboard](dashboard.md)

* [Follow these review guidelines](peerreview.md)

* If the student approves of a submission, then the student will reply to the cover letter
of the patchset with a single line containing the following:

```
Acked-by: $FIRSTNAME $LASTNAME <$USERNAME@winter2025-iit.actc.underground.software>
```

* If the student finds issues with a submission, then the student will reply to the cover letter
of the patchset with detailed feedback about their concerns and conclude the email
with a single line containing the following:

```
Nacked-by: $FIRSTNAME $LASTNAME <$USERNAME@winter2025-iit.actc.underground.software>
```

* In parallel, other students have been assigned the student's submission
and the student should receive feedback from two other students

* These reviews are due 24 hours after the initial submission deadline

* A late or missing submission yields a zero for the review section of the assignment

* If a student does not complete peer review their maximum assignment grade is 80%

* Reviews are graded based on how many issues a student missed.
The student receives 20% off for each unique issue not spotted with max penalty of 100%

**Step 3: Final submission (due _Tuesday, 30 December 2025_)**

* The student, if canny, will act on the feedback from the received reviews

* Regardless of whether the student made changes to their initial submission,
they must make a final submission

* A late or missing final submission will result in a zero grade for the assignment

* This is due 48 hours after the initial submission deadline, and 24 hours after the peer review deadline

* The overall assignment grade is composed of 80% for the final submission and 10% for each peer review

* While the initial submission is not explicitly graded, failure to submit anything or
submissions devoid of any effort whatsoever will result in a zero
