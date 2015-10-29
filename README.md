# Sys-Open-Kernel-Module-Hook
A kernel module that hook's onto the sys_open.

Created in Fedora 20 on 64 bit, with kernel version 3.11.10-301.fc20.x86_64

Changes the sys_call_table to writing aloud, then back to read only. This way, we can override the it and then just call the original call when we are done.
