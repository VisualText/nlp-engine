# NLP Engine

## Tutorial Videos

Many of you have been asking for tutorial videos on NLP++ and here is the first set. More coming soon...
1. NLP++ tutorial videos: http://tutorials.visualtext.org
1. Analyzers used in the videos: https://github.com/VisualText/nlp-tutorials
1. VisualText tutorial videos: http://vttutorials.visualtext.org

## Glitter Chat

[![Join the chat at https://gitter.im/NLPplusplus/NLP-Engine](https://badges.gitter.im/NLPplusplus/NLP-Engine.svg)](https://gitter.im/NLPplusplus/NLP-Engine?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) Join us on Glitter chat!

## About

The NLP engine is the engine that runs text analyzers writtein in [NLP++](http://visualtext.org). The nlp-engine runs on Linux, Windows, and MacOS. It can be called in two different ways:

1. Calling the nlp.exe command line executable (this is what the VSCode NLP++ Language Extension does)
1. Calling from within C++ or another language that can call c++ functions

# Command Line

You can get help on nlp.exe:

      [command arg: --help]

      usage: nlp [--version] [--help]
                 [-INTERP][-COMPILED] INTERP is the default
                 [-ANA analyzer] name or path to NLP++ analyzer folder
                 [-IN infile] input text file path
                 [-OUT outdir] output directory
                 [-WORK workdir] working directory
                 [-DEV][-SILENT] -DEV generates logs, -SILENT (default) does not
                 [infile [outfile]] when no /IN or -OUT specified

      Directories in the nlp.exe files:
         data        nlp engine bootstrap grammar
         analyzers   default location for nlp++ analyzer folders
         visualtext  common files for the VisualText IDE

## Switches

Switch | Function
------------ | -------------
-INTERP / -COMPILED | Runs NLP++ code interpreted or compiled
-ANA | name of the analyzer or path to the analyzer folder
-IN | Input file
-OUT | Output directory
-WORK | Working director where the library and executable files are
-DEV / -SILENT | -DEV generates logs, -SILENT (default) does not
[infile [outfile]] | when no -IN or -OUT specified

# Calling NLP++ Analyzers from C++

There is a test directory in the repository that builds a test.exe that calls analyzers using c++ code.

## Test directory
The test directory contains a call to the nlp engine using buffers.

# Compiling the NLP-ENGINE

The nlp engine compiles and runs on Linux, Windows, and MacOS. The easist way to compile them is using VSCode using the [C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake), and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extensions.

## Linux

To compile the nlp-engine for Linux:

* git clone https://github.com/VisualText/nlp-engine.git
* Install [VSCode](https://code.visualstudio.com/download)
* Open the nlp-engine folder in VSCode
* Install the [C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake), and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extensions into VSCode
* Install the gcc compiler
* Select a "GCC kit" using the bottom status bar
* Click on "Build" in the status bar

## Windows 10

To compile the nlp-engine for Windows 10:

* git clone https://github.com/VisualText/nlp-engine.git
* Install [VSCode](https://code.visualstudio.com/download)
* Open the nlp-engine folder in VSCode
* Install the [C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake), and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extensions into VSCode
* Install the community version of [VisualStudio 2019](https://visualstudio.microsoft.com/downloads/)
* Select a "Visual Studio Community 2019 Release - x86" kit using the bottom status bar
* Click on "Build" in the status bar

## MacOS

To compile the nlp-engine for the Mac:

* git clone https://github.com/VisualText/nlp-engine.git
* Install [VSCode](https://code.visualstudio.com/download)
* Open the nlp-engine folder in VSCode
* Install the [C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake), and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extensions into VSCode
* Install [brew](http://brew.sh) to download mac-specific packages like clang
* Install the [clang c++ compiler](https://clang.llvm.org/get_started.html)
* Select the "Clang kit" using the bottom status bar
* Click on "Build" in the status bar

# NLP-ENGINE Development

All the dependencies for building nlp engine from source are included in this repository. We use VCPkg to checkout and build ICU from source. We use CMake to build the nlp engine making sure the ICU library is "found" through VCPkg.

## NOTE** 
If on WSL and trying to use VSCode to debug, run the following to help with a possible cmake error when trying to build using the C++ and CMake extensions for VsCode:
```
sudo apt-get install libicu-dev
```

Also, on Linux or Mac, you may need to run:
```
sudo apt-get install autoconf-archive
```

## Linux / MacOS
```
git clone --recurse-submodules https://github.com/visualtext/nlp-engine
cd nlp-engine
```

### Fetch the third-party libraries (via vcpkg)
```
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg install
```

### Configure Cmake for out of source build
```
cd ..
cmake -DCMAKE_BUILD_TYPE=Debug -B build -S . -DCMAKE_TOOLCHAIN_FILE='./vcpkg/scripts/buildsystems/vcpkg.cmake'
```

### Build
```
cmake --build build/ --target all
```

Once the build succeedes, the nlp executable should be in the bin folder along with test-nlp.exe.

## Windows

### Clone the repository
```
git clone --recurse-submodules https://github.com/visualtext/nlp-engine
cd nlp-engine
```

### Make sure VCPkg builds 64 bit binaries (Note: VCPkg is expected to make this the default by Sept 2023 after which this step won't be necessary)
```
set VCPKG_DEFAULT_TRIPLET=x64-windows
```

### Fetch the third-party libraries (via vcpkg)
```
cd vcpkg
bootstrap-vcpkg.bat
vcpkg install
```

### Configure Cmake for out of source build
```
cd ..
cmake -G "Visual Studio 16 2019" -A x64 -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Build
```
cmake --build build --config Debug
```

