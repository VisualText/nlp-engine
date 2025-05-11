# Using NLP Engine C++ Library Files in C++

This is example code on how to use the NLP Engine's C++ library codes in a C++ program. There is a CMakeLists file and a mainl.cpp file which incluede and uses the NLP Engine library.

It assumes there are three analyzers in the analzyers folder. The name of the analyzers are the name of the analyzer folders.
The analyzers that appear in this example are legacy and some no longer exist. It is expected that programmers test with
their own set of analyzers.

1. taiparse (it has since been renamed to the parse-en-us)
2. corporate
3. noop (no longer exists)

## Real Usaage

The NLP Engine C++ library files has been used in various places for now: the Python NLPPlus package, and the HPCC Systems super computer plugin.

### Pyhton NLPPlus package

The NLPPlus package uses the NLP Engine C++ libraries and is built using This module is built using the
[scikit-build-core](https://scikit-build-core.readthedocs.io/en/latest/index.html)
and [nanobind](https://nanobind.readthedocs.io/en/latest/index.html) modules. It was originally designed by
Canadian David Huggins-Daines who was contracted by Conceptual Systems.

The presository for the Python NLPPlus package is found here: [https://github.com/VisualText/py-package-nlpengine](https://github.com/VisualText/py-package-nlpengine).

### HPCC Systems

The HPCC Systems NLP++ plugin was designed and coded by David de Hilster while working at LexisNexis Risk. It allows for 
those using the HPCC Systems Supercomputing framework to natively call NLP++ analyzers inside of ECL. 

The repository for the HPCC Systems NLP plugin is found at [https://github.com/hpcc-systems/HPCC-Platform/tree/master/plugins/nlp](https://github.com/hpcc-systems/HPCC-Platform/tree/master/plugins/nlp).

There is a special menu item in VisualText (the VSCode NLP++ language extension) that creates the special "mod"
file for submitting to the HPCC Systems supercomputing cluster. 
