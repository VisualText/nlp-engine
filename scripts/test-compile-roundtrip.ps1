<#
.SYNOPSIS
  Verify that a compiled analyzer produces the same final.tree as the
  interpreted run.

.DESCRIPTION
  Windows counterpart to scripts/test-compile-roundtrip.sh in the Linux/Mac
  sibling repos. Four steps:

    1. nlp.exe interpreted on <input-file>.
       Save the resulting <input>_log/final.tree as
       <analyzer-dir>/final.interpreted.tree.
    2. scripts/compile-analyzer.ps1 — generate C++ + build kbd.dll.
    3. nlp.exe -COMPILED on the same input file.
    4. Byte-by-byte diff the two final.tree files.

  Exit code: 0 if identical, 1 if they differ or any step fails.

.PARAMETER AnalyzerDir
  Analyzer directory (contains spec/, kb/, input/).

.PARAMETER InputFile
  Sample input file. Defaults to <analyzer-dir>\input\text.txt.

.PARAMETER Config
  Debug (default) or Release. Picked to match the nlp.exe + kbd.dll pair.

.PARAMETER EngineRoot
  Root of the nlp-engine checkout. Defaults to the parent of this script.

.EXAMPLE
  scripts\test-compile-roundtrip.ps1 -AnalyzerDir C:\git\analyzers\nlpfix-analyzers\date-time

.EXAMPLE
  scripts\test-compile-roundtrip.ps1 `
      -AnalyzerDir C:\git\analyzers\nlp-tutorials\tutorial-15 `
      -InputFile  C:\git\analyzers\nlp-tutorials\tutorial-15\input\text.txt `
      -Config Debug
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string] $AnalyzerDir,
    [string] $InputFile,
    [ValidateSet('Debug','Release')] [string] $Config = 'Debug',
    [string] $EngineRoot
)

$ErrorActionPreference = 'Stop'

function Fail($msg) { Write-Error $msg; exit 1 }
function Section($msg) {
    Write-Host ""
    Write-Host "============================================================"
    Write-Host $msg
    Write-Host "============================================================"
}

if (-not $EngineRoot -or $EngineRoot -eq '') {
    $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    $EngineRoot = (Resolve-Path (Join-Path $scriptDir '..')).Path
}

$AnalyzerDir = (Resolve-Path $AnalyzerDir).Path
$EngineRoot  = (Resolve-Path $EngineRoot).Path

if (-not $InputFile -or $InputFile -eq '') {
    $InputFile = Join-Path $AnalyzerDir 'input\text.txt'
}
if (-not (Test-Path -LiteralPath $InputFile)) {
    Fail "Input file not found: $InputFile"
}
$InputFile = (Resolve-Path $InputFile).Path

$NlpExe = Join-Path $EngineRoot ("bin\{0}\nlp.exe" -f $Config)
if (-not (Test-Path -LiteralPath $NlpExe)) {
    Fail "nlp.exe not found at $NlpExe. Build the engine first: cmake --build build --config $Config --target nlp"
}

# Same final.tree path the engine writes on every run.
$LogDir    = "$InputFile`_log"   # <input>_log next to the input file
$FinalTree = Join-Path $LogDir 'final.tree'
$SavedTree = Join-Path $AnalyzerDir 'final.interpreted.tree'

function Invoke-Nlp {
    param(
        [Parameter(Mandatory = $true)] [string] $Stage,
        [switch] $Compiled
    )

    if (Test-Path -LiteralPath $LogDir) { Remove-Item -Recurse -Force -LiteralPath $LogDir }

    $args = @()
    if ($Compiled) { $args += '-COMPILED' }
    $args += @('-ANA', $AnalyzerDir, '-WORK', $EngineRoot, $InputFile)

    Write-Host ""
    Write-Host "==> [$Stage] $NlpExe $($args -join ' ')"
    & $NlpExe @args | Out-Null
    if ($LASTEXITCODE -ne 0) { Fail "nlp.exe ($Stage) exited with code $LASTEXITCODE" }

    if (-not (Test-Path -LiteralPath $FinalTree)) {
        Fail "Expected $FinalTree not produced by $Stage run"
    }
}

Section "Round-trip test"
Write-Host "Engine    : $NlpExe"
Write-Host "Analyzer  : $AnalyzerDir"
Write-Host "Input     : $InputFile"
Write-Host "Log dir   : $LogDir"

# -- 1. Interpreted run -------------------------------------------------------
Section "[1/4] Interpreted run"
Invoke-Nlp -Stage 'interpreted'
Copy-Item -Force -LiteralPath $FinalTree -Destination $SavedTree
Write-Host "    Saved interpreted tree -> $SavedTree"

# -- 2. Compile analyzer ------------------------------------------------------
Section "[2/4] Compile analyzer (-> kbd.dll / kb.dll)"
$compileScript = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) 'compile-analyzer.ps1'
& $compileScript -AnalyzerDir $AnalyzerDir -InputFile $InputFile -Config $Config
if ($LASTEXITCODE -ne 0) { Fail "compile-analyzer.ps1 failed (exit $LASTEXITCODE)" }

$dllBase = if ($Config -eq 'Debug') { 'kbd' } else { 'kb' }
$dll = Join-Path $AnalyzerDir ("bin\{0}.dll" -f $dllBase)
if (-not (Test-Path -LiteralPath $dll)) { Fail "Compiled KB not produced: $dll" }

# -- 3. Compiled run ----------------------------------------------------------
Section "[3/4] Compiled run (-COMPILED)"
Invoke-Nlp -Stage 'compiled' -Compiled

# -- 4. Diff ------------------------------------------------------------------
Section "[4/4] Diff final.tree (interpreted vs compiled)"
Write-Host "    interpreted: $SavedTree"
Write-Host "    compiled   : $FinalTree"

$interpBytes   = [System.IO.File]::ReadAllBytes($SavedTree)
$compiledBytes = [System.IO.File]::ReadAllBytes($FinalTree)

$identical = $false
if ($interpBytes.Length -eq $compiledBytes.Length) {
    $identical = $true
    for ($i = 0; $i -lt $interpBytes.Length; $i++) {
        if ($interpBytes[$i] -ne $compiledBytes[$i]) { $identical = $false; break }
    }
}

$hashA = (Get-FileHash -LiteralPath $SavedTree  -Algorithm SHA256).Hash
$hashB = (Get-FileHash -LiteralPath $FinalTree  -Algorithm SHA256).Hash

if ($identical) {
    Write-Host ""
    Write-Host "PASS: interpreted and compiled final.tree are byte-identical."
    Write-Host "      sha256: $hashA"
    exit 0
}

Write-Host ""
Write-Host "FAIL: interpreted and compiled final.tree differ."
Write-Host "      interpreted sha256: $hashA"
Write-Host "      compiled    sha256: $hashB"
Write-Host ""
Write-Host "First 40 differing lines (< interpreted, > compiled):"
$interpLines   = Get-Content -LiteralPath $SavedTree
$compiledLines = Get-Content -LiteralPath $FinalTree
$diff = Compare-Object -ReferenceObject $interpLines -DifferenceObject $compiledLines |
        Select-Object -First 40
$diff | ForEach-Object {
    $sigil = if ($_.SideIndicator -eq '<=') { '<' } else { '>' }
    Write-Host ("    {0} {1}" -f $sigil, $_.InputObject)
}
exit 1
