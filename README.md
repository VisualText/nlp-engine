# NLP Engine

## Tutorial Videos

Many of you have been asking for tutorial videos on NLP++ and here is the first set. More coming soon...
1. NLP++ tutorial videos: http://tutorials.visualtext.org
1. Analyzers used in the videos: https://github.com/VisualText/nlp-tutorials
1. VisualText tutorial videos: http://vttutorials.visualtext.org

## Community

Join the discussion at [https://nlp.discourse.group](https://nlp.discourse.group)

## About

The NLP engine is the engine that runs text analyzers writtein in [NLP++](http://visualtext.org). The nlp-engine runs on Linux, Windows, and MacOS. It can be called in two different ways:

1. Calling the nlp.exe command line executable (this is what the VSCode NLP++ Language Extension does)
1. Calling from within C++ or another language that can call c++ functions

## What's New in Version 4

Version 4 is the release line in which **NLP++ became debuggable**. Version 3 was about how you build, deploy and install an analyzer; Version 4 is about watching one run.

The debugging *experience* lives in the [NLP++ Language Extension for VS Code](https://marketplace.visualstudio.com/items?itemName=dehilster.nlp) — breakpoints, stepping, variables and a call stack, in the editor. What the engine contributes is the machinery underneath it: a small debug server that the extension (or any other client) drives.

### The debug server (`-DEBUG <port>`)

Start the engine with `-DEBUG <port>` and it listens on a loopback socket, speaking newline-delimited JSON — one object per line, both directions. It runs until it reaches a pause point, sends an unsolicited `stopped` event, and blocks reading commands until one of them resumes it.

- **Pause points:** a pass boundary; a rule about to be tried, one that matched, one that failed; and a statement about to run in an `@CODE`, `@POST` or `@DECL` body.
- **Inspection:** the parse tree, the current node and the nodes after it, the rule being tried element by element, the matched elements behind `N(n)`, all five NLP++ variable kinds (`G()`, `L()`, `S()`, `X()`, `N()`), and the calls that led to where execution is.
- **Stepping:** by rule, by match, by pass, and by statement — into a call, over one, or out of the current function.
- **Capability handshake.** A client asks what a build supports (`capabilities`) rather than inferring it from this version string.
- **It costs nothing when off.** Every hook is an inline test of one static `bool`, and the hooks are armed only while the analyzer runs over the input — never while the engine parses its own grammar.

The message catalogue at the top of `lite/nlpdebug.cpp` is the authority on the protocol; the regression driver in `.github/workflows/tests/rule-debugger/` exercises it end to end.

For a fuller narrative of the Version 4 work, see `docs/version-4/VERSION-4-OVERVIEW.md`.

## What's New in Version 3

Version 3 is the release line in which NLP++ became **compilable, cloud-buildable, and installable from package managers**, while hardening the engine across all supported platforms. NLP++ is *glass-box*, **deterministic** NLP — the same input always produces the same output from rules you can read — and Version 3 makes that engine faster, easier to build, and easier to adopt.

### Compiled analyzers (NLP++ → C++ → native library)
- **One-click compilation.** Compiling an analyzer and its knowledge base to a native shared library (`.dll` / `.so` / `.dylib`) was always technically possible but effectively expert-only. Version 3 turns it into a one-click action (in the VS Code extension) with no C++ knowledge required. Use `-COMPILE` to generate the C++, then `-COMPILED` to run the compiled analyzer.
- **Granular compile targets.** `-COMPILE` (analyzer + KB), `-COMPILEKB` (KB only), and `-COMPILEANA` (rules only) so you can recompile just what changed.
- **Compiled Python passes.** A `python` pass anywhere in `analyzer.seq` (including before the tokenizer, e.g. to build a KB/dictionary from JSON) is now emitted into the compiled analyzer and runs from the native library — previously these only worked interpreted.
- **Cross-platform generated code.** The generated C++ compiles and links on Windows, Linux, and macOS; compiled-run dispatch works uniformly across all three.

### Cloud compile
- A **cloud-compile service** builds the native libraries for Windows, Linux, and macOS on hosted runners, so users don't need a local C++ toolchain. Available from the VS Code extension and the language bindings.

### Lazy dictionaries & knowledge bases (large lexicons)
- **`*-full.dict` / `*-full.kbb` files load lazily** — the engine binary-searches the sorted file on disk one word at a time instead of loading the whole lexicon into memory, dramatically cutting memory and startup cost. Multiple lazy files can be open at once.
- Lazy files are **never compiled into the library** — they stay as data files and are stream-searched at runtime, so they work identically in interpreted and compiled analyzers. This lets you ship a compiled analyzer whose only visible data is the `*-full` lexicon.
- New `loadkbb` and `loaddict` NLP++ functions.

### Native language bindings
- **Node.js** — `npm install nlpplus` embeds the engine directly (new in Version 3).
- **Python** — `pip install NLPPlus` native bindings gained the compile / cloud-compile APIs (`compile()`, `cloud_compile()`).

### NLP++ language & robustness
- `_xVAR("attribute")` match-list special for attribute access in rules.
- Digits allowed in underscore-prefixed token names (e.g. `_token2`).
- A long list of compiled-mode correctness fixes (output flushing, null-guards, evaluation-order) and cross-platform build fixes.

For a fuller narrative of the Version 3 work across all the VisualText repos, see the overview in `docs/version-3/VERSION-3-OVERVIEW.md`.

# Command Line

You can get help on nlp.exe:

      [command arg: --help]

      usage: nlp [--version] [--help]
                 [-INTERP][-COMPILED] INTERP is the default
                 [-COMPILE] compile analyzer and KB to C++ (does not run analyzer)
                 [-COMPILEKB] compile only the KB to C++ (does not load grammar or run analyzer)
                 [-COMPILEANA] compile only the analyzer to C++ (does not regenerate the KB or run analyzer)
                 [-ANA analyzer] name or path to NLP++ analyzer folder
                 [-IN infile] input text file path
                 [-OUT outdir] output directory
                 [-WORK workdir] working directory
                 [-DEV][-SILENT] -DEV generates logs, -SILENT suppresses logs/output files (off by default)
                 [infile [outfile]] when no -IN or -OUT specified

      Directories in the nlp.exe files:
         data        nlp engine bootstrap grammar
         analyzers   default location for nlp++ analyzer folders
         visualtext  common files for the VisualText IDE

## Switches

Switch | Function
------------ | -------------
-INTERP / -COMPILED | Runs NLP++ code interpreted or compiled
-COMPILE | Generates C++ code for the analyzer rules and KB (does not run the analyzer). Use -COMPILED after the C++ is compiled externally to run the compiled analyzer.
-COMPILEKB | Generates C++ code for the KB only (does not load the grammar or run the analyzer). Use when only the KB changed.
-COMPILEANA | Generates C++ code for the analyzer rules only, skipping KB regeneration (does not run the analyzer). Use when only the rules changed and the KB is already compiled.
-ANA | name of the analyzer or path to the analyzer folder
-IN | Input file
-OUT | Output directory
-WORK | Working director where the library and executable files are
-DEV / -SILENT | -DEV generates logs, -SILENT suppresses logs/output files (off by default)
-DEBUG | Port to serve the debug protocol on. The engine waits for a client, then stops at pass boundaries, rule attempts and statements so the client can step and inspect. Used by the VS Code extension's debugger; see "What's New in Version 4".
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

All the dependencies for building nlp engine from source are included in this repository. We use VCPkg to checkout and build ICU from source. We use CMake to build the nlp engine making sure the dependencies are properly referenced. There are other ways to build the nlp engine, but this is the standard method.

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

## Cross-repo release automation

Analyzer / parse-en-us updates auto-propagate across the VisualText repos via
GitHub `repository_dispatch`. See **[docs/PERCOLATION.md](docs/PERCOLATION.md)**
for the full map — the chain, event-types, the `CLASSIC_PAT` secret, and the
`vcpkg` caveat.
