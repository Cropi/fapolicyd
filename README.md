File Access Policy Daemon
=========================

[![Build Status](https://travis-ci.com/linux-application-whitelisting/fapolicyd.svg?branch=master)](https://travis-ci.com/linux-application-whitelisting/fapolicyd)

This is a simple application whitelisting daemon for Linux.

DEPENDENCIES (fedora)
---------------------
* gcc
* autoconf
* automake
* libtool
* make
* libudev-devel
* kernel-headers
* systemd-devel
* libgcrypt-devel
* rpm-devel
* file-devel
* libcap-ng-devel
* libseccomp-devel
* lmdb-devel
* python3-devel

BUILDING
--------
To build from the repo after cloning:

```
$ cd fapolicyd
$ ./autogen.sh
$ ./configure
$ make
$ make dist
```

This will create a tarball. You can use the new tarball with the spec file
and create your own rpm. If you want to experiment without installing, just
run make with no arguments. It should run fine from where it was built as
long as you put the configuration files in /etc/fapolicyd. The fapolicyd.rules
and fapolicyd.mounts files go there.


RUNNING
-------
You need to setup the fapolicyd.mounts file because every system
is potentially partitioned differently. The daemon has to place a watch
on each partition in order to get events occurring on the partition.
If the daemon does not get notifications, then it cannot make access
control decisions.

To get an initial watch list, you can run this:

```
mount | egrep '^tmpfs| ext4| ext3| xfs' | awk '{ printf "%s\n", $3 }' >> /etc/fapolicyd/fapolicyd.mounts
```

You might want to look at the fapolicyd.rules file to see what the sample
policy looks like. The policy is designed with 4 goals in mind.

1. No bypass of security by executing programs via ld.so.
2. All approved executables are packaged. Unpackaged programs can't run
3. Elf and python files/shared objects must come from system directories.
This prevents LD_LIBRARY & PYTHON_LIBRARY redirection to an attacker
controlled dir.
4. Other languages are not allowed or must be enabled.

You can test by starting the daemon from the command line. Before starting
the daemon, cp /usr/bin/ls /usr/bin/my-ls just to setup for testing. When
testing new policy, its highly recommended to use the permissive mode to
make sure nothing bad happens. It really is not too hard to deadlock your
system. Continuing on with the tutorial, as root start the daemon as follows:
```
/usr/sbin/fapolicyd --permissive --debug
```
Then in another window do the following:

1. /usr/lib64/ld-2.22.so /usr/bin/ls
2. my-ls
3. run a python file in your home directory.
4. run a program from /tmp

In permissive + debug mode you will see dec=deny which means
"decision is to deny". But the program will actually be allowed to run.

You can run the daemon from the command line with --debug-deny command
line option. This culls the event notification to only print the denials.
If this is running cleanly, then you can remove the --permissive option
and get true denials. Now retest above steps and see the difference.


DEBUG MODE
----------
In debug mode, you will see events such as this:

```
rule:4 dec=deny auid=1000 pid=7792 exe=/usr/bin/my-ls file=/etc/ld.so.cache
```

What this is saying is rule 4 made the ultimate Decision that was followed.
The Decision is to deny access. The subject is the user that logged in as
user id 1000. The subject's process id that is trying to perform an action
is 7792. The current executable that subject is using is my-ls. The my-ls
application wanted to access /etc/ld.so.cache which is the object.


WRITING RULES
-------------
The rules follow a simple "decision subject object" recipe. For more
information, see the fapolicyd.rules man page.


REPORT
------
On shutdown the daemon will write an object access report to
/var/log/fapolicyd-access.log. The report is from oldest access to newest.
Timestamps are not included because that would be a severe performance hit.
The report gives some basic forensic information about what was being accessed.


NOTES
-----
Its highly recommended to run in permissive mode while you are testing the
daemon's policy.

Stracing the daemon can deadlock the system.

About shell script restrictions...there's not much difference between
running a script or someone typing things in by hand. The aim at this
point is to check that any programs it calls meets the policy.

If for some reason rpm database errors are detected, you may need to do
the following:

1. db_verify /var/lib/rpm/Packages
if OK, then
2. rm -f /var/lib/rpm/__db*
3. rpm --rebuilddb

