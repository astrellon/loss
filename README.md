# loss

LOSS (Lua Operating SystemS)

Aim: To be able to run many virtual kernels built to run Lua.

## Building

Currently LOSS shouldn't require anything other than clang and libncurses which doesn't always come pre-installed for all distros.
```
sudo apt-get install clang libncurses5-dev
```

Building should be as simple as calling
```
make
./Loss
```

Unit testing is as simple as
```
make TESTING=yes
./LossTests
```

FUSE tool
Todo: Extend tool to be able to create empty harddrives.
```
sudo apt-get install libfuse-dev

make FUSE=yes
./Lossfs <name of harddrive file> <mount folder>
```

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
