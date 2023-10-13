# SDL + OpenGL Game Template

## Prerequisites
- `CMake` VERSION 3.15^

## Submodules
Use this command to automatically clone all submodules *(after cloning this project)*
```sh
$ git submodule update --recursive --init --force
```

## Using VScode
For the best VScode experience install the
[`C/C++ Extension Pack`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack).

## Building the Game
`~` refers to the directory `readme.md` is in.
```sh
$ cmake -S ~ -B ~/build

$ cmake --build ~/build --config <config>
```
`<config>` can be `Release` or `Debug` *(default)*
