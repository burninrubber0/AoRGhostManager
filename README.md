# Art of Rally Ghost Manager
A simple solution for sharing ghosts in Art of Rally

## Usage
Go to File > Open, choose your ghosts file, then choose another player's ghosts file. It may take a few seconds to load.

The left table can be altered by the + and - buttons in the center. - removes selected entries from the left table. + adds selected entries from the right table to the left table.

## Building
Requires CMake and Qt6. Qt should be in PATH.

```
mkdir build
cd build
cmake ..
cmake --build .
```