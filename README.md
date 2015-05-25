loss
====

LOSS (Lua Operating SyStem)

Building
--------

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
