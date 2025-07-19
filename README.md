<img align = "left" src = "res/HENSDKLogo.svg" width = "125px">

# HEN SDK

[![Windows Build](https://github.com/AnkeDoesCoding/HENSDK/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/AnkeDoesCoding/HENSDK/actions/workflows/cmake-windows.yml) 
<br>
[![Linux Build](https://github.com/AnkeDoesCoding/HENSDK/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/AnkeDoesCoding/HENSDK/actions/workflows/cmake-linux.yml)

<br>

HEN SDK is a small WIP SDK for 3D games. It is currently a learning project so it is recommended that you don't use this for a full-feature game.



## Building

### Supported Platforms

HEN SDK supports Windows and Linux (only Ubuntu has been tested so far).

### Prerequisiteries

It is recommended that you have [VSCode](https://code.visualstudio.com/). <br>
You will need a compiler for your platform:

| Platform | Compiler | Link |
| ------------- | ------------- | ------------- |
| Windows | MSVC | [Visual Studio](https://visualstudio.microsoft.com/downloads/?q=build+tools#build-tools-for-visual-studio-2022) | 
| Linux | Clang | [Clang](https://releases.llvm.org/download.html) | 

<br>

You will also need the following:
* [CMake](https://github.com/Kitware/CMake) <br>
* [Git](https://git-scm.com/downloads)
* `libgl-dev` (if your on Linux)

### Configuring and Compiling

Once you have everything that is needed, you should be able to clone the repository and open it up in VSCode. <br> 
From VSCode, you can select the configuration preset, configure and compile the SDK.
