# What is this library for?

This library was written to solve the problem that applications
which use the systemd journald API for logging won't log anything
without systemd. libsystemd0 simply doesn't fall back to syslog
if systemd is unavailable.

This library only implements sd_journal_* functions which produce
logging output and will forward everything to syslog.

It is not allowed to use this library in an argument to encourage
others to use the systemd journal API. Doing so violates the license
terms, which means that you loose all rights to the software granted
by the license if you do so.

This library is ment for software for which the mistake of using the
systemd journal API was already made. It allows users of such software
to get log messages again, and gives developers who used the systemd
journal API a way to make the transitition back to syslog easier.

That there is no header file for the implemented functions is intentional,
it discourages others to use the library functions without any reason.


# Compilation

Just type `make` and a static and a dynamic library will be created
inside the /out/ folder.


# Usage

If you have a program which uses the systemd journal API, and it doesn't
produce any logs, you can use this library with LD_PRELOAD.
```
LD_PRELOAD=/path/to/libjournal.so your_program
```

If you can't use LD_PRELOAD, be it because it is a suid binary or for other
reasons, or if you want to use this library automatically, you can add the
path to the library to /etc/ld.so.preload. This will cause the library to
always get loaded for any program, regardles if it uses any of it's
functionality. Adding the library to /etc/ld.so.preload isn't adviced and
should only be done as last resort, even though it probably doesn't have
a big impact on system ressources. If you add a library to /etc/ld.so.preload,
make sure that the library, the directory it is in, and /etc/ld.so.preload
are only writable by root.

This library can also be used during compilation of a program. Just add the
library before the systemd0 library to use its logging functions instead of
systemds. This is mainly useful for maintainers of distributions which don't
use systemd but want their programs to generate log messages anyway. This is
also useful for developers who want to migrate their software back to syslog.
