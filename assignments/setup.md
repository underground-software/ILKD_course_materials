## Setup ⚙️

Welcome to the class! Setting up a Linux environment
and compiling your own kernel is required for the course,
and this assignment will serve as a guide to get you started.
Use this as an opportunity to introduce yourself to the class,
and get your feet wet with the `git send-email`
patch submission process we'll be using.

#### Outcomes:

* Set up your Linux environment for the rest of the class

* Get comfortable with building the kernel

* Get familiar with git and git patches

* Get familiar with how to submit to the mailing list via `git send-email`

* Get familiar with the peer review process, which we will use for the rest of the course

#### What to submit:

* Patch 1 adds `$USER/setup/setup.txt` which contains a brief introduction about yourself

* Patch 2 adds `$USER/setup/qemu.txt` which contains the output of your qemu vm booting your new kernel

    * The first line of this file contains your new version string described below

    * The last line of this file is "reboot: Power down"

* Don't forget a cover letter

* Submit your patches to `setup@fall2025-uml.kdlp.underground.software`

#### Procedure:

0. Install [Podman](https://podman.io)

    * Windows: from the above link, select `Podman CLI for Windows` from the `Download` dropdown menu.

        * Select WSLv2 as your virtualization provider

        * Check the `Install WSLv2 if not present` box

        * You will also need to create a `.ssh` folder in your User directory if it doesn't already exist

            * Open the file explorer and navigate to `C:\Users\$YOUR_WINDOWS_USERNAME`

            * Add a new folder and name it `.ssh`

    * MacOS: from the above link, select `Podman CLI for MacOS` from the `Download` dropdown menu.

        * Double click `podman-installer-macos-universal.pkg` in your downloads folder once it is done downloading

        * Run through the install wizard

    * Linux: Install Podman through your package manager

        * Note: if you are using Ubuntu or a distribution based on Ubuntu like Linux Mint you will need to jump through some hoops because Ubuntu does not yet package podman version 5

            0. Install podman from your package manager (`sudo apt install podman`)

            0. Check the installed version with `podman --version`. If the version is greater than 5.0.0 you are all set and can continue with the next steps and stop reading these special instructions

            0. Otherwise, run `podman run --hostname builder --name builder --privileged -it --rm quay.io/podman/stable`

            0. Within that container, run the linux command from the step "Build your container using the Containerfile we serve you"

            0. Wait for the build to finish and for the shell prompt to reappear.

            0. Open a second terminal and execute `podman exec builder podman image save kdlp_container:latest | podman image load`

            0. Once that command finishes, close the second terminal and go back to the first terminal. Type `exit` and hit enter

            0. Proceed with the rest of the instructions starting with "Create a podman volume for persistent storage"

0. Open a terminal

    * Linux: open your terminal

    * MacOS: search for `terminal`

    * Windows: search for `cmd`

0. (Windows & MacOS only) Initialize and start the Podman machine

        podman machine init
        podman machine start

0. Build your container using the Containerfile we serve you:

    Linux/MacOS:

        sh -c 'read -rp "username: " username && curl -u $username https://fall2025-uml.kdlp.underground.software/Containerfile | podman build -t kdlp_container -'

    Windows:

        cmd /v /c "set /p "username=username: " && curl -u !username! https://fall2025-uml.kdlp.underground.software/Containerfile | podman build -t kdlp_container -"

    If you want `vim` to be the default editor in your container instead of nano, include the query parameter `?vim=yes` at the end of the URL after `/Containerfile`.

0. Create a podman volume for persistent storage

        podman volume create kdlp_volume

0. Get into the container

        podman run -it --rm --hostname kdlp --name kdlp -v kdlp_volume:/home kdlp_container

0. Clone the submission repository

        git clone https://fall2025-uml.kdlp.underground.software/cgit/submissions

0. Create the `$USER/setup` directory and subdirectory

        cd submissions
        mkdir -p $USER/setup
        cd $USER/setup

0. Add a file named setup.txt to this folder containing an introduction about yourself.

    The content can be whatever you want, whether it be why you are taking this class,
    your favorite ice cream flavor, or a fun fact about yourself.
    Use whichever text editor you prefer.

        nano setup.txt # or: vim setup.txt

0. Save the file and exit your text editor

0. Make a commit out of your changes

    By default, git will not be tracking changes to newly created files.
    Add your file to the list that git is tracking with `git add setup.txt`.

    Verify that you did not introduce any whitespace errors by running `git diff --staged --check`.
    You can read more about whitespace errors on the [commits](/faq/commits.md) page. They can (and should)
    be something you check for at the end before you send your patches, but the above command can be used as a preventative measure
    to avoid introducing them in the first place when it is easier to fix. If the command complains, change the offending lines and
    `git add setup.txt` again and check once more. Repeat until no errors remain.

    Make a commit to save this version of the repository, so it can be shared with `git commit -s`.
    Note that the -s flag makes git include the "Signed-off-by:" or DCO line for you automatically.
    See our page on [patches](/faq/patches.md) for more information on the DCO.

    Git will then open an instance of the default text editor (nano unless you used the `?vim=yes` parameter when building your container) to let you input a message for the commit.
    Put a title containing a short summary of what you did on the first line, e.g. `setup: add introduction for Linus Torvalds`. Press enter twice, and then write a more
    detailed explanation that will act as the body of the commit.

    There should already be a "Signed-off-by:" line for your account at the bottom.
    If not, add one, then save your changes and exit the editor to finish the commit.

    Check to make sure you see the commit and it looks good by running `git log -p`.
    Your new commit should be the top most one, and you should see the title,
    message, DCO, and difference view containing the changes.

    If there is more output than can fit on one screen, git will open a scrolling view that you
    can maneuver up and down within using the arrow keys. Press q to go back to the terminal.

0. Go home

        cd

0. Copy the provided `init.config` configuration file and `init.s` source listing from the submissions folder into your home directory

        cp ~/submissions/setup/init.* .

0. Make a directory called `rootfs` and compile the init program, outputting to this rootfs directory

        mkdir rootfs
        riscv64-linux-gnu-gcc -static-pie -ffreestanding -nostdlib init.s -march=rv64i -mabi=lp64 -o rootfs/init

0. Create a root filesystem image

        cd rootfs && find . | cpio -co > ../rootfs.cpio && cd ..

0. Clone the v6.16 Linux kernel release from git.kernel.org. It's recommended that you
set `--depth=1` to minimize the download size.

        git clone --depth=1 --branch v6.16 https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

0. Enter the `linux` directory

        cd linux

0. Create a branch to make it easier to track your future work.
You will notice a message about being in a "detached head state".
When the `--branch` argument to git clone is supplied with a tag (v6.16 in this case), git doesn't make a branch automatically.

        git checkout -b master

0. Create a file named `localversion` in the `linux` directory and add something cool.
We suggest it begin with a '.' character, and it must NOT contain spaces.
For example, `cat localversion` might print

        .my-cool-kernel-version

    which will result in a kernel version like

        6.16.0.my-cool-kernel-version

0. Make a complete config from the provided config file seed

        ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make KCONFIG_ALLCONFIG=../init.config allnoconfig

0. Build the kernel

        ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- make -j $(nproc) Image

0. Boot!

        qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -kernel arch/riscv/boot/Image -initrd ../rootfs.cpio

    At the top of the qemu output you will see the linux kernel version, along with the custom version string you set before in `localversion`.
    At the bottom, shortly before the kernel messages about powering down, you will the message "Hello!" that was printed by the sample init program.

0. Extra credit opportunity

    edit the provided init program to make it interactive, prompt the user for their name by printing a message, then set up a buffer and use the read system call to
    get a line of input. Print hello followed by echoing back out whatever the user entered to create a custom greeting. The output might look something like

        What is your name? <type in e.g. Linus Torvalds and hit enter>
        Hello, Linus Torvalds

    If you choose to do this extra credit, be sure to leave comments in your code since assembly can be hard to read without them. Copy your modified `init.s` file back
    into your `~/submissions/$USER/setup/` folder. You will make a commit containing that file as an additional third commit at the end.

0. Re-run `qemu` to capture the output in a file you will submit as patch 2.
This command uses a shell pipe and the tee program to send a second copy of the output to the standard error stream so it still appears on your terminal while also forwarding it along to the next stage in the pipeline.
The `tr` program then deletes the carriage return characters that would cause whitespace errors.
The final stage uses output redirection to save the processed text into a file in your folder.

        qemu-system-riscv64 -machine virt -bios none -nographic -no-reboot -net none -kernel arch/riscv/boot/Image -initrd ../rootfs.cpio | tee /dev/stderr | tr -d '\r' > ~/submissions/$USER/setup/qemu.txt

0. Create another commit containing just the new `$USER/setup/qemu.txt` file

        cd ~/submissions
        git add $USER/setup/qemu.txt
        git diff --staged --check #check for whitespace errors, you might have some to fix. Edit the file, re run git add and check until there are none left
        git commit -s

0. If you completed the extra credit, make a commit containing just the `init.s` file.

       git add $USER/setup/init.s
       git commit -s

0. Next, you'll want to create a patch series, also known as a [patchset](/faq/patchsets.md), with a [cover letter](/faq/coverletters.md) out of your [commits](/faq/commits.md).
To do this, run `git format-patch -2 --cover-letter -v1 --rfc` (or substitute `-3` for `-2` if you did the extra credit).

    * `-2` includes the most recent 2 commits (use 3 if you did the extra credit)

    * `--cover-letter` generates a cover letter for the whole patch series with a summary

    * `-v1` marks this as the first version of this patch set

    * `--rfc` marks these patches as a request for comment, which is required for all initial submissions (since they will be receiving peer feedback)

0. The result of this command should be your directory containing three (or four) new `.patch` files, courtesy of git.
You'll want to open the first of these, `v1-0000-cover-letter.patch`,
in your text editor so that you can write your cover letter according to the [guidelines](/faq/coverletters.md).
Don't forget to add the `Signed-off-by` line at the end of the body right before the automatically generated section containing a summary of the patchset (the first line will have your name and the number of patches in parenthesis e.g. Linus Torvalds (2):).

0. Once you've finished your cover letter, send your patches to the class mailing list.
You can do this by running the command `git send-email --to=setup@fall2025-uml.kdlp.underground.software v1*.patch`.

    * Each assignment will have its own special address to send submissions to, in this case `setup@fall2025-uml.kdlp.underground.software`

    * The expression `v1*.patch` will be expanded by the shell into all file names matching
    that pattern (any file whose name starts with `v1` and ends with `.patch`)

    * `git send-email` will prompt you to ask whether it should send the emails, typing `a` and hitting enter will send them all

        * If it is successful, the output should end with `Result: 250` which indicates that the server accepted your emails

        * If not, **do not hesitate to reach out** in `#questions` on the course Matrix so that we can help with troubleshooting

If your `git send-email` output ends in `Result: 250` then your patchset was sent successfully.

#### Check your work

You can check the Course Dashboard on the course website to confirm your submission was received.
On the dashboard there is a section for each assignment that has been assigned.
Look for the section labeled `setup`. In the table beneath it,
if you see a `Timestamp` and `Submission ID` in their respective cells,
your submission was received successfully. If instead, you see ` - ` in those fields,
your submission was not received.

After submitting all later assignments, you can check the course dashboard to make sure your submission was successfully received.

[Frequently Asked Questions](/faq.md)
