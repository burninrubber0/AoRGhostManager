# Art of Rally Ghost Manager
A simple solution for sharing ghosts for Art of Rally.

## Usage
Choose two ghosts files, either via the lines at the bottom of the window or File > Open.

The player's ghosts can be added to by selecting new ghosts and using the << button. Changes can be reverted via the >> button.

Due to technical limitations, the current version of Ghost Manager cannot remove ghosts from the player file. This feature will be added in the future.

## Building
Requires CMake and Qt6. Qt should be in PATH.

```
mkdir build
cd build
cmake ..
cmake --build .
```