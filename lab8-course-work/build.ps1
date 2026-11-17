param(
    [Parameter(Mandatory = $true)][string]$QtPath,
    [Parameter(Mandatory = $true)][string]$MinGWPath,
    [switch]$Test,
    [switch]$Deploy,
    [switch]$Run
)

$ErrorActionPreference = 'Stop'
$qtDirectory = (Resolve-Path -LiteralPath $QtPath).Path
$compilerDirectory = (Resolve-Path -LiteralPath $MinGWPath).Path
$compiler = Join-Path $compilerDirectory 'bin/g++.exe'
$make = Join-Path $compilerDirectory 'bin/mingw32-make.exe'
if (!(Test-Path -LiteralPath $compiler) -or !(Test-Path -LiteralPath $make)) {
    throw 'MinGWPath must contain bin/g++.exe and bin/mingw32-make.exe.'
}
if (!(Test-Path -LiteralPath (Join-Path $qtDirectory 'bin/Qt6Widgets.dll')) -and
    !(Test-Path -LiteralPath (Join-Path $qtDirectory 'bin/Qt5Widgets.dll'))) {
    throw 'QtPath must point to the Qt MinGW kit, for example C:/Qt/6.x.x/mingw_64.'
}

$oldPath = $env:Path
try {
    $env:Path = "$qtDirectory/bin;$compilerDirectory/bin;$oldPath"
    $cmake = (Get-Command cmake -ErrorAction Stop).Source
    $buildDirectory = Join-Path $PSScriptRoot 'build-mingw'
    $testsEnabled = if ($Test) { 'ON' } else { 'OFF' }
    & $cmake -S $PSScriptRoot -B $buildDirectory -G 'MinGW Makefiles' `
        "-DCMAKE_PREFIX_PATH=$qtDirectory" "-DCMAKE_CXX_COMPILER=$compiler" `
        "-DCMAKE_MAKE_PROGRAM=$make" '-DCMAKE_BUILD_TYPE=Release' "-DBUILD_TESTING=$testsEnabled"
    if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
    & $cmake --build $buildDirectory --parallel
    if ($LASTEXITCODE -ne 0) { throw 'Compilation failed.' }
    if ($Test) {
        $ctest = Join-Path (Split-Path $cmake) 'ctest.exe'
        Push-Location -LiteralPath $buildDirectory
        try {
            & $ctest --output-on-failure
            if ($LASTEXITCODE -ne 0) { throw 'Tests failed.' }
        } finally { Pop-Location }
    }
    $program = Join-Path $buildDirectory 'PhoneBook.exe'
    if ($Deploy) {
        & (Join-Path $qtDirectory 'bin/windeployqt.exe') --release $program
        if ($LASTEXITCODE -ne 0) { throw 'Qt deployment failed.' }
    }
    if ($Run) { & $program }
} finally {
    $env:Path = $oldPath
}
