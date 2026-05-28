<#
.SYNOPSIS
  Compile an NLP++ analyzer's KB into kb.dll / kbd.dll for -COMPILED runs.

.DESCRIPTION
  Windows counterpart to scripts/compile-analyzer.sh in nlp-engine-linux and
  nlp-engine-mac. Three steps:

    1. Run nlp.exe -COMPILE on the analyzer + sample input. The engine emits
       Cc_code.cpp, Sym*/Con*/Ptr*/St*.cpp, kb_setup.h, and kb_setup.cpp into
       <analyzer>/kb/. kb_setup is decorated with __declspec(dllexport) so
       MSVC exports it from the resulting DLL (see cc_gen.cpp).
    2. Synthesize <analyzer>/.nlp-compile/src/StdAfx.h + CMakeLists.txt and
       build the KB shared library in the matching MSVC config.
    3. Drop the artifact at <analyzer>/bin/<name>.dll, which is the exact path
       cs/libconsh/cg.cpp LoadLibrarys at -COMPILED time. The name depends on
       the engine build config:
         Debug   -> kbd.dll   (Debug   + UNICODE -> kbdu.dll)
         Release -> kb.dll    (Release + UNICODE -> kbu.dll)

  Engine static libs (prim, kbm, consh, words, lite) come from the local
  nlp-engine build tree at <repo>/lib/<Config>/. ICU import libs come from
  vcpkg_installed/x64-windows/lib/. Headers come from include/ and cs/include/.

.PARAMETER AnalyzerDir
  Path to the analyzer directory (the one that contains spec/ and input/).

.PARAMETER InputFile
  A sample input passed to nlp.exe -COMPILE. The compile pass doesn't analyze,
  but the engine still requires the arg to be present and resolvable.

.PARAMETER Config
  Debug (default) or Release. Picks the matching engine .lib set and names
  the output DLL kbd.dll vs kb.dll.

.PARAMETER EngineRoot
  Root of the nlp-engine checkout. Defaults to the parent of this script.

.EXAMPLE
  scripts\compile-analyzer.ps1 -AnalyzerDir C:\git\analyzers\nlpfix-analyzers\date-time `
                               -InputFile C:\git\analyzers\nlpfix-analyzers\date-time\input\text.txt
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string] $AnalyzerDir,
    [Parameter(Mandatory = $true)] [string] $InputFile,
    [ValidateSet('Debug','Release')] [string] $Config = 'Debug',
    [string] $EngineRoot
)

$ErrorActionPreference = 'Stop'

function Fail($msg) { Write-Error $msg; exit 1 }

# Resolve EngineRoot relative to this script if not supplied.
if (-not $EngineRoot -or $EngineRoot -eq '') {
    $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    $EngineRoot = (Resolve-Path (Join-Path $scriptDir '..')).Path
}

$AnalyzerDir = (Resolve-Path $AnalyzerDir).Path
$InputFile   = (Resolve-Path $InputFile).Path
$EngineRoot  = (Resolve-Path $EngineRoot).Path

# Engine build outputs (multi-config MSVC layout from CMakeLists.txt).
$EngineBin    = Join-Path $EngineRoot ("bin\{0}" -f $Config)
$EngineLibDir = Join-Path $EngineRoot ("lib\{0}" -f $Config)
$NlpExe       = Join-Path $EngineBin 'nlp.exe'

if (-not (Test-Path -LiteralPath $NlpExe)) {
    Fail "nlp.exe not found at $NlpExe. Build the engine first: cmake --build build --config $Config --target nlp"
}
foreach ($name in 'prim','kbm','consh','words','lite') {
    if (-not (Test-Path -LiteralPath (Join-Path $EngineLibDir "$name.lib"))) {
        Fail "Engine static lib missing: $EngineLibDir\$name.lib. Build target '$name' first."
    }
}

# Vcpkg toolchain — drives find_package(ICU) into the correct Debug/Release
# import libs (and matching DLL deployment) for the chosen $Config.
$VcpkgToolchain = Join-Path $EngineRoot 'vcpkg\scripts\buildsystems\vcpkg.cmake'
if (-not (Test-Path -LiteralPath $VcpkgToolchain)) {
    Fail "Vcpkg toolchain not found at $VcpkgToolchain. Run: vcpkg\bootstrap-vcpkg.bat then vcpkg\vcpkg.exe install --triplet x64-windows"
}

# Output DLL name the engine LoadLibrarys (cs/libconsh/cg.cpp line ~157+).
# We're not using UNICODE in the engine build, so kbd.dll / kb.dll.
$DllBase = if ($Config -eq 'Debug') { 'kbd' } else { 'kb' }

Write-Host ""
Write-Host "==> [1/3] nlp.exe -COMPILE  ($AnalyzerDir)"
Write-Host "         engine=$NlpExe"
Write-Host ""

# Run -COMPILE. Working dir = engine root so the engine can find data/.
# -WORK passes the same value through to the engine.
& $NlpExe -COMPILE -ANA $AnalyzerDir -WORK $EngineRoot $InputFile
if ($LASTEXITCODE -ne 0) { Fail "nlp.exe -COMPILE exited with code $LASTEXITCODE" }

$KbDir = Join-Path $AnalyzerDir 'kb'
$GeneratedCpp = @(Get-ChildItem -LiteralPath $KbDir -Filter '*.cpp' -ErrorAction SilentlyContinue)
if ($GeneratedCpp.Count -eq 0) {
    Fail "No generated .cpp files in $KbDir. Did -COMPILE succeed?"
}

# Build tree under <analyzer>/.nlp-compile/{src,build}.
$BuildRoot = Join-Path $AnalyzerDir '.nlp-compile'
$SrcDir    = Join-Path $BuildRoot 'src'
$BuildDir  = Join-Path $BuildRoot ("build-{0}" -f $Config)
if (Test-Path -LiteralPath $BuildRoot) { Remove-Item -Recurse -Force -LiteralPath $BuildRoot }
New-Item -ItemType Directory -Force -Path $SrcDir | Out-Null

# Generated sources `#include "StdAfx.h"`. Mirror what the engine's own libs
# use (cs/libconsh/StdAfx.h, plus the kb template): <tchar.h> brings in _TCHAR,
# my_tchar.h brings in the _t_ostream / _T() macros referenced by Cc_code.cpp,
# streamClasses.h is referenced by consh/cg.h.
Set-Content -LiteralPath (Join-Path $SrcDir 'StdAfx.h') -Encoding utf8 -Value @'
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <my_tchar.h>
#include <streamClasses.h>
'@

# Build CMakeLists.txt. CMake paths use forward slashes on Windows to avoid
# escape headaches inside the here-string.
$AnaCmake          = $AnalyzerDir -replace '\\','/'
$EngineRootCmake   = $EngineRoot -replace '\\','/'
$EngineLibDirCmake = $EngineLibDir -replace '\\','/'
$SrcDirCmake       = $SrcDir -replace '\\','/'

$cmakeLists = @"
cmake_minimum_required(VERSION 3.16)
project(nlp_kb_library LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ICU comes via the vcpkg toolchain (passed on the cmake command line).
# find_package picks the Debug or Release variant to match the active config,
# which keeps the on-disk DLL deps in lockstep with the deployed icu*d?78.dll.
find_package(ICU REQUIRED COMPONENTS i18n uc data io)

# Drop the artifact at <analyzer>/bin/<name>.dll (engine's hardcoded path).
set(KB_OUT_DIR "$AnaCmake/bin")
file(MAKE_DIRECTORY "`${KB_OUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "`${KB_OUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "`${KB_OUT_DIR}")
foreach(c Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER `${c} cu)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_`${cu} "`${KB_OUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_`${cu} "`${KB_OUT_DIR}")
endforeach()

file(GLOB GENERATED_CPP "$AnaCmake/kb/*.cpp")
if(NOT GENERATED_CPP)
    message(FATAL_ERROR "No generated .cpp files in $AnaCmake/kb/")
endif()

add_library(nlp_kb SHARED `${GENERATED_CPP})
set_target_properties(nlp_kb PROPERTIES
    OUTPUT_NAME "$DllBase"
    PREFIX ""
)

target_include_directories(nlp_kb PRIVATE
    "$SrcDirCmake"
    "$AnaCmake"
    "$AnaCmake/kb"
    "$EngineRootCmake/include"
    "$EngineRootCmake/include/Api"
    "$EngineRootCmake/include/Api/lite"
    "$EngineRootCmake/cs/include"
)

target_compile_definitions(nlp_kb PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _CRT_NON_CONFORMING_SWPRINTFS
    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    NONSTD_
    _CONSOLE
    WIN32
    _WINDOWS
    # The engine's own sub-CMakeLists pass MSVC_VERSION (= CMake's MSVC_VERSION
    # built-in, e.g. 1941). prim.h:33 uses it to skip the legacy
    # #define INFINITY for VS 2015+. Without it the guard sees 0 < 1900 and
    # redefines INFINITY, triggering C4005 against ucrt's corecrt_math.h.
    MSVC_VERSION=`${MSVC_VERSION}
)
target_compile_options(nlp_kb PRIVATE /FI"StdAfx.h" /Zc:wchar_t)

target_link_directories(nlp_kb PRIVATE
    "$EngineLibDirCmake"
)

# Engine static libs. consh + words + kbm depend on prim; lite is top-level.
target_link_libraries(nlp_kb PRIVATE
    prim kbm consh words lite
    ICU::i18n ICU::uc ICU::data ICU::io
)
"@

Set-Content -LiteralPath (Join-Path $SrcDir 'CMakeLists.txt') -Encoding utf8 -Value $cmakeLists

Write-Host ""
Write-Host "==> [2/3] cmake configure  ($BuildDir)"
# VCPKG_INSTALLED_DIR points the vcpkg toolchain at the already-populated
# packages tree under the engine root so find_package(ICU) succeeds without
# requiring a vcpkg.json next to our generated CMakeLists.
$VcpkgInstalled = Join-Path $EngineRoot 'vcpkg_installed'
& cmake -S $SrcDir -B $BuildDir -A x64 `
    "-DCMAKE_TOOLCHAIN_FILE=$VcpkgToolchain" `
    "-DVCPKG_INSTALLED_DIR=$VcpkgInstalled" `
    "-DVCPKG_TARGET_TRIPLET=x64-windows"
if ($LASTEXITCODE -ne 0) { Fail "cmake configure failed (exit $LASTEXITCODE)" }

Write-Host ""
Write-Host "==> [3/3] cmake --build --config $Config"
& cmake --build $BuildDir --config $Config
if ($LASTEXITCODE -ne 0) { Fail "cmake build failed (exit $LASTEXITCODE)" }

$Out = Join-Path $AnalyzerDir ("bin\{0}.dll" -f $DllBase)
if (Test-Path -LiteralPath $Out) {
    Write-Host ""
    Write-Host "Built: $Out"
    Write-Host "Run:   $NlpExe -COMPILED -ANA `"$AnalyzerDir`" -WORK `"$EngineRoot`" `"$InputFile`""
    Write-Host ""
} else {
    Fail "Expected output $Out was not produced."
}
