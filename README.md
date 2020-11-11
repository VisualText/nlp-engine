# nlp-engine

The NLP engine is the engine that runs text analyzers writtein in [NLP++](http://visualtext.org). It can be called in two different ways:

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
                 [-DEV][-SILENT] /DEV generates logs, -SILENT (default) does not
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
