General list
============

FileSystem
----------
- Expand the metadata and user aspects.
- FUSE for mounting.

Users
-----
- User permissions
| - Root user
- User data
- Storing and loading user data

Processes
---------
- A single process
- Multiple processes
- Inter-Process-Communication
- Shared Memory

Devices
-------
- TTY
- Graphics
- Sound?



Thoughts:
- TTY
  Kernel level TTYs?
  - 

Scheduler
---------

First version, round robin.
Need a way of having anything that would block normally (stream reading) puts the process to sleep until that
handle has data.
