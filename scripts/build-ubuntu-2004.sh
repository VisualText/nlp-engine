#!/usr/bin/env bash
# Build the Ubuntu 20.04 nlp binary + compile-libs inside an ubuntu:20.04
# container. Invoked by .github/workflows/build-linux.yml via `docker run`
# (replacing addnab/docker-run-action, whose docker:20.10 helper image pull
# from Docker Hub was intermittently timing out and failing the job).
#
# Runs with the repo checked out and mounted at the working directory.
set -e
export DEBIAN_FRONTEND=noninteractive
export TZ=Etc/UTC
apt update
apt install -y \
  git build-essential cmake g++ curl unzip zip tar \
  autoconf-archive pkg-config python3 ninja-build libicu66 libicu-dev
rm -rf analyzers   # drop the pinned submodule copy; test wants fresh analyzers master
git clone --recurse-submodules https://github.com/VisualText/analyzers.git analyzers
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ..
mkdir -p rfb-logs data/rfb/spec analyzers/parse-en-us/output
cp ./bin/nlp ./bin/nlpl.exe
echo "Copying ICU 66 libraries..."
mkdir -p icu-libs
cp /usr/lib/x86_64-linux-gnu/libicu*.so.66* ./icu-libs/
zip -r icu-libs.zip icu-libs
./bin/nlp --version || true
./bin/nlp -ANA ./analyzers/parse-en-us -WORK ./ ./analyzers/parse-en-us/input/doj.txt -LOG rfb-logs -DEV || true

echo "Packaging compile libraries..."
rm -rf compile-libs
mkdir -p compile-libs/include/Api compile-libs/lib
cp -r include/Api/. compile-libs/include/Api/
cp -r cs/include compile-libs/include/cs
for name in prim kbm consh words lite; do
  lib=$(find . -type f -name "lib${name}.a" -not -path "./vcpkg/*" -not -path "./compile-libs/*" | head -n1)
  if [ -z "$lib" ]; then
    echo "Missing required static library: lib${name}.a" >&2
    exit 1
  fi
  cp "$lib" "compile-libs/lib/lib${name}.a"
done
# System ICU 66 runtime libs (dynamic) used by the 20.04 build.
cp /usr/lib/x86_64-linux-gnu/libicu*.so.66* compile-libs/lib/
(cd compile-libs && zip -r ../nlpengine-compile-libs-linux-20.04.zip .)
