# nlp-engine

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

The nlp engine compiles and runs on Linus, Windows, and MacOS. The easist way to compile them is using VSCode using the [C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake), and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extensions.

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
* Install the [clang c++ compiler](https://clang.llvm.org/get_started.html)
* Select the "Clang kit" using the bottom status bar
* Click on "Build" in the status bar
