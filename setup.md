## Setup ⚙️

Welcome to the class! Setting up a Linux environment
and compiling your own kernel is required for the course,
and this assignment will serve as a guide to get you started.
Use this as an opportunity to introduce yourself to the class,
and get your feet wet with the `git send-email` patch submission process we'll
be using.

#### Outcomes:

* Set up your Linux environment for the rest of the class

* Get comfortable with building the kernel

* Get familiar with git and git patches

* Get familiar with how to submit to the mailing list via `git send-email`

* Get familiar with the peer review process, which we will use for the rest of the course

#### What to submit:

* Patch 1 adds `username/setup/setup.txt` which contains a brief introduction
about yourself and the output of `uname -a`

* Patch 2 adds the output of your qemu vm, which should contain your new version string. FIXME: settle on what proof will look like

* Don't forget a cover letter

* Submit your patches to `setup@spring2025-utsa.kdlp.underground.software`

#### Procedure:

0. Install [Podman](https://podman.io)
    * Windows: from the above link, select `Podman CLI for Windows` from the `Download` dropdown menu.

        * FIXME: better instructions? I already have WSL set up, I don't know how things differ with a new WSL install.

        * Select WSLv2 as your virtualization provider

        * Check the `Install WSLv2 if not present` box

    * MacOS: from the above link, select `Podman CLI for MacOS` from the `Download` dropdown menu.

        * Double click `podman-installer-macos-universal.pkg` in your downloads folder once it is done downloading

        * Run through the install wizard

    * Linux: Install Podman through your package manager

0. Open a terminal

    * Windows: search for `cmd`

    * MacOS: search for `terminal`

    * Linux: open your terminal


0. Windows & MacOS

   * Initialize and start the Podman machine

            podman machine init
            podman machine start

0. Build your container using the Containerfile we provide you
FIXME: Provide Containerfile + command to build using it

        podman build ...

0. FIXME: get into container, navigate to whatever directory...

0. Clone the course git repo

        git clone https://spring2025-utsa.kdlp.underground.software/cgit/ILKD_Submissions/  FIXME:? course git repo link?

0. Go into the course repo directory, and create a directory in the repo where `your-username` is
the username you use to log into the course website.
FIXME: is this the way ILKD_Submissions is set up? I forget.

        cd ILKD_Submissions
        mkdir your-username
        cd your-username

0. Add a file named setup.txt to this folder containing an introduction about yourself.
The content can be whatever you want, whether it be why you are taking this class, your favorite ice cream flavor, or a fun fact about yourself.
Use whichever text editor you prefer. `nano` has keybindings that should be familiar to most people.

        nano setup.txt

0. Save and exit your text editor

0. Append a line with information about the container environment you set up to the end of the file.

        uname -a >> setup.txt

0. Make a commit out of your changes

    By default git will not be tracking changes to newly created files.
    Add your file to the list that git is tracking with `git add setup.txt`, then make a commit to save this version of the repository
    so it can be shared with `git commit -s`. Note that the -s flag makes git include the Signed-off-by DCO line for you automatically.


    Git will then open an instance of your preferred text editor to let you input a message for the commit. Put a title containing a
    short summary of what you did on the first line, e.g. `Setup: Added introduction for so and so`. Press enter twice, and then write a more
    detailed explanation that will act as the body of the commit.


    There should already be a Signed-off by line for your account at the bottom. If not, add one, then save your changes and exit the editor to finish the commit.


    Check to make sure you see the commit and it looks good by running `git log -p`. Your new commit should be the top most one, and you should see the title,
    message, DCO, and difference view containing the changes.


    If there is more output than can fit on one screen, git will open a scrolling view that you can maneuver up and down within using the arrow keys. Press q to go back to the terminal.


0. Navigate out of the ILKD_Submissions directory

        cd ../.. FIXME: or cd to /whatever_dir_is_automatically_set_up

0. Clone the v6.12 Linux kernel release from git.kernel.org. It's recommended that you
set `--depth=1` to minimize the download size.
FIXME: version?

        git clone --depth=1 --branch v6.12 https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

0. Copy the provided `whatever.config` file and `init.S` init program into the `linux` dir
FIXME: provide config, init program?

        cp whatever.config init.S linux/

0. Change the current directory to the kernel tree with

        cd linux

0. Create a file named `localversion` in the `linux` directory and add something cool.
We suggest it begin with a '.' character, and it must NOT contain spaces.
For example, `cat localversion` might return

        .my-cool-kernel-version

    which will result in a kernel version like

        6.12.0.my-cool-kernel-version

0. Make a complete config from the provided config file

        ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make KCONFIG_ALLCONFIG=whatever.config allnoconfig

0. Build the kernel

        ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- time make -j $(nproc)

0. Build the init program
FIXME: init program ??

        mkdir rootfs
        riscv64-linux-gnu-gcc -pie -ffreestanding -nostdlib init.S -static -march=rv64i -mabi=lp64 -shared -o rootfs/init

0. Create a root filesystem

        cd rootfs && find . | cpio -co > ../rootfs.cpio && cd ..

0. Boot!

        qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -net none -kernel arch/riscv/boot/Image -initrd rootfs.cpio

0. To exit qemu, press `Ctrl-a`, then press `x`
FIXME: should the vm shutdown properly at this point?

0. At the top of the qemu output you will see the linux kernel version, along with the custom version string you set before in `localversion`.
FIXME: copy all of the output? Copy some of the output? Run vm again but redirect output to append to the setup.txt file?

0. Navigate back to your `your-username` directory

0. Create another commit with the new changes to your `setup.txt`. You can either manually add the `setup.txt` changes with `git add setup.txt`, or use the
`-a` flag with `git commit` to automatically add all changes to tracked files to the current commit.

        git commit -as

0. Next, you'll want to create a patch series with a cover letter out of your commit.
To do this, run `git format-patch -2 --cover-letter -v1 --rfc`

    * `-2` includes the most recent 2 commits

    * `--cover-letter` generates a cover letter for the whole patch series with a summary

    * `-v1` marks this as the first version of this patch set

    * `--rfc` marks these patches as a request for comment, which is required for all initial submissions (since they will be receiving peer feedback)

0. The result of this command should be your directory containing three new `.patch` files, courtesy of git.
You'll want to open the first of these, `v1-0000-cover-letter.patch`,
in your text editor so that you can write your cover letter according to the
[assignment submission guidelines](../procedures.md#cover-letter-guidelines).
Don't forget to add the `Signed-off-by` line at the end of the body.

0. Once you've finished your cover letter, send your patches to the class mailing list.
You can do this by running the command `git send-email --to=setup@spring2025-utsa.kdlp.underground.software v1*.patch`.

    * Each assignment will have its own special address to send submissions to, in this case `setup@spring2025-utsa.kdlp.underground.software`

    * The expression `v1*.patch` will be expanded by the shell into all file names matching
    that pattern (any file whose name starts with `v1` and ends with `.patch`)

    * `git send-email` will prompt you to ask whether it should send the emails, typing `a` and hitting enter will send them all

        * If it is successful, the output should end with `Result: 250` which indicates that the server accepted your emails

        * If not, **do not hesitate to reach out** in `#questions` on the course Matrix so that we can help with troubleshooting

#### Check your work
Open the email list by running the `mutt` command.

In `mutt`, you will see a list of email threads, which should begin
with a "welcome to the email system" message,
followed by all subsequent emails in chronological order.

Press:

* `enter` to view which message you have highlighted

* `q` while viewing a message to exit back to the main screen.

* `space` to scroll to the next page within the email.

* `q` or `ctrl+c` to exit `mutt`.

Before the initial submission deadline, you will see no messages.
Following the deadline, all submissions are revealed and peer review begins.

If your `git send-email` output ends in `Result: 250` then, congratulations! You successfully completed the setup.

You can check the Course Dashboard on the course website to confirm your submission was received.
On the dashboard there is a section for each assignment that has been assigned.
Look for the section labeled `setup`. In the table beneath it,
if you see a `Timestamp` and `Submission ID` in their respective cells,
your submission was received successfully. If instead, you see ` - ` in those fields,
your submission was not received.

After submitting all later assignments, you can check the course dashboard to make sure your submission was successfully received.
