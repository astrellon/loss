# loss

LOSS (Lua Operating SystemS)

Aim: To be able to run many virtual kernels built to run Lua.

## Building

Currently LOSS shouldn't require anything other than clang and libncurses which doesn't always come pre-installed for all distros.
```
sudo apt-get install clang libncurses5-dev libboost-system1.62-dev libboost-context1.62-dev libboost-coroutine1.62-dev libfuse-dev libsfml-dev libboost1.62-dev
```

Building should be as simple as calling
```
make
./LossProg <name of harddrive file>
```

Unit testing is as simple as
```
make TESTING=yes
./LossTests
```

FUSE tool
Todo: Extend tool to be able to create empty harddrives.
```
./LossFuse -d <name of harddrive file> <mount folder>
```

## Features
- A modified version of lua using boost coroutines to support pre-emptive multitasking.
- Mostly supports and has a concept of multiple processes.
- Supports a VFS (virtual filesystem). So everything starts up in a RAMFS and then harddrives are mounted.
- Uses CMake and generally seems to work across different platforms.

## Flaws
- Not really stable.
- Fuse tool has some strange issues overriding files.
- Has more dependancies than it really needs to build.

## Example
```
[ SUCCESS ] Setup VFS
[ SUCCESS ] Created root user
[ SUCCESS ] Created kernel process
[ SUCCESS ] Created proc filesystem
Booting system from init.d

Lua worked
Ending process: lua: 4
Reading /greetings.txt
Line! 1: Hello There
Line! 2: How are you?
Line! 3:
Line! 4: What is up?

Lua worked
Ending process: lua: 4
```
