# Art of Rally Ghost Manager
A simple solution for sharing ghosts for Art of Rally.

## Usage
Choose two ghosts files, either via the lines at the bottom of the window or File > Open.

The player's ghosts file can be added to by selecting new ghosts and using the << button. Changes can be reverted via the >> button. Ghosts can be removed from a file via Tools > Ghost Remover.

## Building
Requires CMake and Qt6. Qt should be in PATH.

```
mkdir build
cd build
cmake ..
cmake --build .
```