## Introduction and Course Setup

Welcome to the class! Setting up a Linux environment is required for the course,
and this assignment will serve as a guide to get you started.
Use this as an opportunity to introduce yourself to the class, and get your feet wet with the `git send-email` patch submission process we'll be using.

#### What to submit:

* Patch 1 adds `username/setup.txt` containing an introduction about yourself.

* Patch 2 adds the updates to `username/setup.txt` including the output of `whoami`, `uname -a`, and `df -H`

* Don't forget a cover letter

* All email code patches are expected to pass `checkpatch.pl` as described in the [policies and procedures](/procedures.md)

* Submit your patches to setup@COURSE_DOMAIN

### Install Necessary Software

Start by downloading [the standard ISO image for Fedora Server](https://getfedora.org/en/server/download) for your architecture.
You'll likely want the ISO for x86\_64 unless you have an M-series Mac, or you are running Linux on ARM, in which case you want the aarch64 ISO.
The ISO is large, so the download may take a while depending on your internet speed.

Next, download and install hypervisor software for your operating system:

* Windows - [VirtualBox](https://www.virtualbox.org/wiki/Downloads):

    * VirtualBox also works on Intel Macs

* MacOS - [UTM](https://mac.getutm.app/):

    * Use the download link instead of the App Store link, the software is identical
but they charge $10 for it on the App Store

* Linux - [virt-manager](https://virt-manager.org/):

    * You can most likely get it from your system's package manager

### Set up the VM

Once the ISO image has finished downloading, you can begin the VM set-up process.
The exact procedure will vary depending on the software you're using, but
the core idea is that you'll want to create a new virtual machine guest and decide what portion of your machine's resources to allocate to the VM.

Most likely you can follow the set-up wizard of your software for creating a new guest
and end up with a working machine, but you probably want to increase the default values
for RAM, CPU cores, and disk space. The more you can give the VM, the better, but a good
rule of thumb is to leave at least half of your cores and RAM for your host system.

Feel free to reach out for specific advice depending on your machine.

Be sure to allocate **at least 50G** for the virtual hard drive.

At some point in the setup process, you'll be prompted to attach the ISO image to the machine,
which will allow it to boot into the Fedora live environment.
You can then start the VM for the first time and it will boot the Fedora ISO image.

### Install Fedora to Your VM

Once the VM boots you will see a grub menu with two options:

* test this media and install fedora

* install fedora

You can select either one;
testing the media will verify that your ISO downloaded correctly, but it will take longer.

Once the actual operating system boots, the Fedora installer should open automatically.
Before you begin the installation you **must** resolve the two items with orange warning icons shown in the below image:

* Click on **Root Account**, enable it,
and then set up root with a password you'll remember before clicking "Done"

* Click the **Installation Destination** icon, then click "Done" in the resultant menu
    * You don't need to change anything--it just wants you to confirm the default hard-drive
and partitioning selections it has made

<img alt="fedora confusion image" src="/images/fedora_confusion.png"></img>

If you get stuck, there are numerous tutorials online on how to proceed based on the VM software you are using.
Note that on Fedora 40 you may see "User Creation" with a similar warning icon,
but that warning will be resolved when you configure the root account,
and can be safely ignored.

Once the installation has finished, Fedora will tell you to reboot.
Shut down the machine and remove the ISO image from where you attached it to the VM
(once this is done and you confirmed the installation was successful,
you can remove the ISO image from your host machine to free up disk space).

Next, boot up the VM. You should be greeted with a Fedora login prompt;
use the username "root" and then enter the root password you selected during installation
(it won't look like you are typing any characters when you enter the password, but they are being received--hiding them is just a security measure).

If you logged in correctly, you should be greeted with a command prompt that looks something like:

```
[root@localhost ~]# _
```

If you see this, you have completed the basic installation of Fedora correctly. Congrats!

### Post-Install Setup

First, set a hostname for the machine.
While logged in as root, run the following commands:

* `echo your-hostname > /etc/hostname` where you replace `your-hostname` with whatever name you'd like to use (e.g. `joels-fedora-vm`)

* `hostname --file /etc/hostname` to update it without needing to restart the machine

To verify, log out, and the login prompt will include the new hostname before the word login
(e.g. `joels-fedora-vm login: _`).

You'll then want to run a software update. While logged in on your root account,
run `dnf update -y` (this may take a while), and reboot when it completes.

If you'd like to continue following these steps instead of waiting for this to complete,
you can switch to a different console by pressing `ctrl+alt+f2`
(on some keyboards you might need a function key to press f2), which will open the second tty.

You can check back on the update any time by pressing `ctrl+alt+f1`, which will switch you back to your original tty.

The next step is to create a non-root account for yourself with sudo permissions.
While logged in as root, run

* `useradd username` where `username` is whatever username you'd like to have

* `usermod -aG wheel username` to give `username` permission to run commands as the superuser (root)

* `passwd username` which will prompt you to create a password for `username`

You can then log out of root (type `exit` or hit `ctrl+d`)
and try to log in as your new user with the username and password you selected.

Once logged in, you'll want to verify that you have root access. Running `sudo whoami`
should prompt you for a password which, once entered,
will allow the `whoami` command to execute and output `root`.

Although you allocated the requisite 50+G of space, Fedora defaults to only using 15G for your filesystem. You can resize it to use all available space with:
```
$ lvextend -r -l +100%free /dev/fedora/root
```

**At this point, you can go no further without** `dnf update -y` **finishing.
If it's still running, let it finish, reboot, and then continue.**

### Configure Class Email

Login as your non root user account, then run `sudo dnf install -y git git-email`
to install the necessary packages for this step.

When this has finished, configure your `.gitconfig`, which lives in your home directory `~/`, using a text editor (e.g. `nano ~/.gitconfig` or `vi ~/.gitconfig`).

You can pick your default editor by adding
```
[core]
    editor = nano # Or which ever editor you prefer
```
then set your identity and account information by adding
```
[user]
		name = Your Name Here
		email = YOUR_USERNAME@COURSE_DOMAIN
[sendemail]
		smtpUser = YOUR_USERNAME
		smtpPass = YOUR_PASSWORD
		smtpserver = COURSE_DOMAIN
		smtpserverport = 465
		smtpencryption = ssl
```

Make sure to fill in the fields with your class username and password (the ones you use to log into the course website).

Next, clone
[the assignment git repository](https://fall2024-uml.kdlp.underground.software/cgit/ILKD_Submissions/).
Use `cd` to enter the directory, then `mkdir your-username`, to create a directory in the repo, where `your-username` is the username you use to log into the course website.

Add a file named `setup.txt` to this folder containing an introduction about yourself.
The content can be whatever you want, whether it be why you are taking this class, your favorite ice cream flavor, or a fun fact about yourself.

When you're done, make a commit out of your changes.
By default git will not be tracking changes to newly created files.
Add your file to the list that git is tracking with `git add setup.txt`, then
make a commit to save this version of the repository so it can be shared with `git commit -s`.
Note that the `-s` flag makes git include the `Signed-off-by` DCO line for you automatically.

Git will then open an instance of your preferred text editor to let you input a message for the commit.
Put a title containing a short summary of what you did on the first line,
e.g. `Setup: Added introduction for so and so`.
Press enter twice,
and then write a more detailed explanation that will act as the body of the commit.

There should already be a `Signed-off-by` line for your account at the bottom. If not, add one, then save your changes and exit the editor to finish the commit.

Check to make sure you see the commit and it looks good by running `git log -p`.
Your new commit should be the top most one, and you should see the title, message, DCO,
and difference view containing the changes.

If there is more output than can fit on one screen, git will open a scrolling view that you can maneuver up and down within using the arrow keys. Press `q` to go back to the terminal.

Now, run:

* `whoami >> setup.txt` to append your chosen local username

* `uname -a >> setup.txt` to append a line with information about the VM environment you set up

* `df -H >> setup.txt` to append your available disk space

Add your file to the list that git is tracking once again using `git add setup.txt`
and make another commit with these changes. Don't forget to add a title, detailed description,
and verify the `Signed-off-by:` line is included in the commit.

Next, you'll want to create a patch series with a cover letter out of your commit.
To do this, run `git format-patch -2 --cover-letter -v1 --rfc`

* `-2` includes the most recent 2 commits

* `--cover-letter` generates a cover letter for the whole patch series with a summary

* `-v1` marks this as the first version of this patch set

* `--rfc` marks these patches as a request for comment, which is required for all initial submissions (since they will be receiving peer feedback)

The result of this command should be your directory containing three new `.patch` files,
courtesy of git.
You'll want to open the first of these, `v1-0000-cover-letter.patch`,
in your text editor so that you can write your cover letter according to the
[assignment submission guidelines](../procedures.md#cover-letter-guidelines).

Don't forget to add the `Signed-off-by` line at the end of they body.

Once you've finished your cover letter, send your patches to the class mailing list.
You can do this by running the command
`git send-email --to=setup@COURSE_DOMAIN v1*.patch`.

* Each assignment will have its own special address to send submissions to, in this case `setup@COURSE_DOMAIN`

* The expression `v1*.patch` will be expanded by the shell into all file names matching
that pattern (any file whose name starts with `v1` and ends with `.patch`)

* `git send-email` will prompt you to ask whether it should send the emails, typing `a` and hitting enter will send them all

If it is successful, the output should end with `Result: 250` which indicates that the server accepted your emails.

If not, **do not hesitate to reach out** in `#questions` on the course Matrix so that we can help with troubleshooting.

### Check Your Work

While logged in as your non root user account, install `mutt` (a terminal program for viewing email) by running `sudo dnf install -y mutt`.
Next, configure your `.muttrc` which lives in `~/`, using a text editor (e.g. `nano ~/.muttrc` or `vi ~/.muttrc`).
First you'll want to set your identity and account information by adding:

```
set realname="Your Name Here"
set my_username="YOUR_USERNAME"
set my_password="YOUR_PASSWORD"
set my_domain="COURSE_DOMAIN"
```

Fill in your credentials that you use for the website and your full name.
Next, set some sane defaults, and configure the server addresses by adding:

```
set spoolfile=
set record=
set folder=
set sort=reverse-threads
set from="$my_username@$my_domain"
set header_cache=~/.cache/mutt
set smtp_url="smtps://$my_username:$my_password@$my_domain:465"
push "<change-folder>pops://$my_username:$my_password@$my_domain:995"\n
macro index l "|git am -s"\n
```

Copy that block in verbatim
and `mutt` will substitute the variables you set in the previous declarations for you.
Once that's out of the way, open the email list by running the `mutt` command.

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

After submitting all later asssignments, you can check the course dashboard to make sure your submission was successfully received.

### Optional VM Configuration

You may decide you'd like to better prepare your VM for the remainder of the course.
Here are some recommended steps you might take:

1. **Set up SSH access.** While logged in to the machine on the tty,
run the command `ip -br a | grep UP` and read the ip address listed there.
On your host machine, open a terminal and try running `ssh username@ip-address`
where `username` is the username you picked earlier and `ip-address` is the one you just found.
<br><br>
    The first time you connect to a new server,
`ssh` will not know the server and will say it cannot establish the authenticity of the machine you are connecting to, asking if you are sure you want to continue connecting.
Type `yes` and hit enter.
<br><br>
    If you're using VirtualBox, you may need to setup port forwarding to enable 'ssh' access. To do this, navigate to the network settings of your VM, then create a new port forwarding rule. Set the **Host Port** to 2222 and the **Guest Port** to 22. Next, you can start your VM and connect via `ssh` using `ssh -p 2222 your-username@127.0.0.1`
<br><br>
    You will be prompted for your password--this is the password for your account on the VM.
Once entered, you will be able to access your VM from your host machine
through a shell exactly like the tty you get by logging in within the VM window.
You can log out by typing `exit` and hitting enter, or pressing `ctrl+d` just like the tty.
<br><br>
    If you want to obviate the need to enter the password every time you log in, you can set up SSH-keys.
Open a terminal on your host machine (`cmd.exe` on windows).
If you have never set up SSH-keys (you might have done it already if you use ssh for github or gitlab), run the command `ssh-keygen` in a terminal on your host machine and accept the default values. Once done, (or if you already have SSH-keys):

    * On linux or MacOS, run `ssh-copy-id username@ip-address` to copy your keys to the VM.

    * On Windows, run `type $env:USERPROFILE\.ssh\id_rsa.pub | ssh username@ip-address "cat >> .ssh/authorized_keys"`
<br><br>

    You may be prompted for your VM password one final time. Enter it, and the command should finish.
Now, try logging in with `ssh username@ip-address` to make sure you are no longer being
prompted for your password.

1. **Make the VM headless.** If you only ever plan on using your VM via SSH,
you can lighten the load on your computer and increase the speed at which the VM runs
by disabling graphical output.
<br><br>
    The exact process for this will vary depending on
which hypervisor you're using, but after turning the VM off with `sudo poweroff`,
the idea is to look through the VM settings for some kind of "graphical output" checkbox.

1. **Easy whitespace spotting.** If you are a `vim` user,
you can save yourself some substantial hassle chasing down whitespace errors
by adding the following lines to your `~/.vimrc` file:

        :highlight ExtraWhitespace ctermbg=red guibg=red
        :match ExtraWhitespace /\s\+$/
