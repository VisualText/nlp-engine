# DESC: Test fixture. Writes marker.txt into the analyzer directory.
#
# The engine invokes a python pass as:
#     <interpreter> spec/<name>.py <appdir> <inputfile> pre|post
# so argv[1] is the analyzer directory. Writing a file there is the simplest
# observable proof that the pass actually executed -- report.nlp reads it back
# with readfile() and the CI step asserts on the result.
import sys, os

appdir = sys.argv[1] if len(sys.argv) > 1 else "."
phase  = sys.argv[3] if len(sys.argv) > 3 else "?"

with open(os.path.join(appdir, "marker.txt"), "w") as f:
    f.write("PYTHON-RAN-" + phase)
