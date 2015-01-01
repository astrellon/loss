loss
====

LOSS (Lua Operating SyStem)

Building
--------

Currently LOSS shouldn't require anything other than g++ which doesn't always come pre-installed for all distros.
```
sudo apt-get install g++
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
