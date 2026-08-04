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

# Focal (20.04) reached end of standard support in May 2025 and
# archive.ubuntu.com no longer serves it reliably: the base "focal" and
# "focal-updates" InRelease fetches now fail from multiple mirror IPs
# ("Connection failed [IP: 91.189.91.82 80]"). apt then reports every
# package from those pockets as "has no installation candidate" -- which
# reads like a missing package but is really a missing index -- and the
# build dies at `apt install` before compiling anything.
#
# Retry first (the failure was intermittent before it became persistent),
# then fall back to old-releases.ubuntu.com, which is where Ubuntu keeps
# EOL releases. Check functionally -- can apt actually resolve a package --
# rather than scraping apt's log format.
apt_can_resolve() {
  apt-get update >/dev/null 2>&1 || true
  apt-cache policy build-essential 2>/dev/null | grep -q 'Candidate: [^(]'
}

apt_ok=0
for attempt in 1 2 3; do
  if apt_can_resolve; then apt_ok=1; break; fi
  echo "[apt] focal index unavailable (attempt $attempt/3); retrying in 15s..."
  sleep 15
done

if [ "$apt_ok" -ne 1 ]; then
  echo "[apt] falling back to old-releases.ubuntu.com (focal is EOL)"
  sed -i -e 's|http://archive.ubuntu.com/ubuntu|http://old-releases.ubuntu.com/ubuntu|g' \
         -e 's|http://security.ubuntu.com/ubuntu|http://old-releases.ubuntu.com/ubuntu|g' \
         /etc/apt/sources.list
  apt-get update
  if ! apt-cache policy build-essential 2>/dev/null | grep -q 'Candidate: [^(]'; then
    echo "[apt] ERROR: no package index available from archive or old-releases." >&2
    exit 1
  fi
fi

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
