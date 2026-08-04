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

# Focal (20.04) went EOL for standard support in May 2025, and the generic
# archive.ubuntu.com is no longer reliable for it from GitHub's runners:
# InRelease fetches fail with "Connection failed [IP: 91.189.91.82 80]"
# from several mirror IPs, and often only SOME pockets come down. apt then
# reports the packages it could not index as "has no installation
# candidate" / "Unable to locate package", which reads like a bad package
# list but really means a missing index -- every name below is correct.
#
# Two things matter here:
#   * Mirror. GitHub's own runner images rewrite sources to
#     azure.archive.ubuntu.com because the runners live in Azure and the
#     generic archive is slow/flaky from there. The ubuntu:20.04 container
#     does not inherit that, so do it explicitly.
#   * What we verify. An earlier version of this checked "can apt resolve
#     build-essential" as a proxy for a healthy index. That is not enough:
#     a PARTIAL index resolves build-essential while zip, pkg-config,
#     python3, ninja-build and autoconf-archive are still missing, so the
#     check passed and the install failed anyway. Retry the actual install
#     -- the only thing whose success we care about -- instead of a proxy.
PKGS="git build-essential cmake g++ curl unzip zip tar
      autoconf-archive pkg-config python3 ninja-build libicu66 libicu-dev"

use_mirror() {   # $1 = hostname to point every ubuntu pocket at
  sed -i -E "s|https?://[a-z.]*archive\.ubuntu\.com/ubuntu|http://$1/ubuntu|g; \
             s|https?://security\.ubuntu\.com/ubuntu|http://$1/ubuntu|g" \
         /etc/apt/sources.list
}

apt_install_all() {
  apt-get update >/dev/null 2>&1 || true
  apt-get install -y $PKGS
}

# Azure mirror first (fast and reliable from GitHub runners), generic
# archive next, then old-releases, which is where Ubuntu parks EOL
# releases once they leave the main archive.
installed=0
for mirror in azure.archive.ubuntu.com archive.ubuntu.com old-releases.ubuntu.com; do
  use_mirror "$mirror"
  for attempt in 1 2; do
    echo "[apt] installing build deps from $mirror (attempt $attempt/2)..."
    if apt_install_all; then installed=1; break; fi
    sleep 15
  done
  [ "$installed" -eq 1 ] && { echo "[apt] ok via $mirror"; break; }
  echo "[apt] $mirror could not supply all packages; trying next mirror..."
done

if [ "$installed" -ne 1 ]; then
  echo "[apt] ERROR: no Ubuntu mirror could supply the build dependencies." >&2
  exit 1
fi
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
