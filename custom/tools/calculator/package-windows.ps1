param(
    [Parameter(Mandatory)][string]$BuildDirectory,
    [Parameter(Mandatory)][string]$QtRoot,
    [Parameter(Mandatory)][string]$OutputDirectory
)
$ErrorActionPreference = 'Stop'
$source = $PSScriptRoot
$output = (New-Item -ItemType Directory -Force -Path $OutputDirectory).FullName
$stage = Join-Path $output 'payload'
$reports = (New-Item -ItemType Directory -Force -Path (Join-Path $output 'reports')).FullName
cmake --install $BuildDirectory --config Release --prefix $stage
if ($LASTEXITCODE -ne 0) { throw 'Runtime deployment failed' }

# App-local CRTs also cover Qt plugins that the executable does not link directly.
$redist = Join-Path $env:VCToolsRedistDir 'x64/Microsoft.VC143.CRT'
if (-not (Test-Path -LiteralPath $redist)) { throw "MSVC runtime missing: $redist" }
Copy-Item -Path (Join-Path $redist '*.dll') -Destination (Join-Path $stage 'bin') -Force

$sourceArchive = Join-Path $output 'CaacCalculator-1.0.0-source.zip'
git archive --format=zip "--output=$sourceArchive" HEAD custom/tools/calculator custom/src/PlanView/CaacCalculatorMath.js LICENSE-GPL LICENSE-APACHE
if ($LASTEXITCODE -ne 0) { throw 'Source archive failed' }
New-Item -ItemType Directory -Force -Path (Join-Path $stage 'source') | Out-Null
Copy-Item -LiteralPath $sourceArchive -Destination (Join-Path $stage 'source')
$sourceCommit = (git rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0) { throw 'Unable to identify source revision' }
$sourceCommit | Set-Content -LiteralPath (Join-Path $stage 'source-revision.txt') -Encoding ascii

# Preserve the Qt modules' license texts and third-party attribution files, without bundling SDKs.
$qtVersion = (& (Join-Path $QtRoot 'bin/qmake.exe') -query QT_VERSION).Trim()
if ($LASTEXITCODE -ne 0) { throw 'Unable to identify Qt version' }
foreach ($module in @('qtbase', 'qtdeclarative')) {
    $notices = Join-Path $env:RUNNER_TEMP "calculator-notices-$module"
    git clone --depth 1 --filter=blob:none --sparse --branch "v$qtVersion" "https://github.com/qt/$module.git" $notices
    if ($LASTEXITCODE -ne 0) { throw "Qt notice source download failed: $module" }
    @('/LICENSES/', '**/LICENSE*', '**/LICENCE*', '**/COPYING*', '**/COPYRIGHT*', '**/qt_attribution.json') |
        git -C $notices sparse-checkout set --no-cone --stdin
    if ($LASTEXITCODE -ne 0) { throw "Qt notice checkout failed: $module" }
    $destination = Join-Path $stage "licenses/qt/$module"
    foreach ($file in Get-ChildItem -LiteralPath $notices -Recurse -File) {
        $relative = [IO.Path]::GetRelativePath($notices, $file.FullName)
        if ($relative.StartsWith('.git')) { continue }
        $target = Join-Path $destination $relative
        New-Item -ItemType Directory -Force -Path (Split-Path $target) | Out-Null
        Copy-Item -LiteralPath $file.FullName -Destination $target
    }
    if (-not (Test-Path -LiteralPath (Join-Path $destination 'LICENSES/GPL-3.0-only.txt'))) {
        throw "Qt license text missing: $module"
    }
}

$binary = Join-Path $stage 'bin/CaacCalculator.exe'
$testBinary = Join-Path $stage 'bin/CalculatorTest.exe'
Copy-Item -LiteralPath (Join-Path $BuildDirectory 'Release/CalculatorTest.exe') -Destination $testBinary
$testRuntime = Join-Path $stage 'bin/Qt6Test.dll'
Copy-Item -LiteralPath (Join-Path $QtRoot 'bin/Qt6Test.dll') -Destination $testRuntime
$windowsPlugin = Get-ChildItem -LiteralPath $stage -Recurse -Filter qwindows.dll | Select-Object -First 1
if (-not $windowsPlugin) { throw 'Windows platform plugin missing from deployment' }
Copy-Item -LiteralPath (Join-Path $QtRoot 'plugins/platforms/qoffscreen.dll') -Destination $windowsPlugin.DirectoryName -Force
$savedPath = $env:PATH
$savedPlatform = $env:QT_QPA_PLATFORM
$savedPlugins = $env:QT_PLUGIN_PATH
try {
    $env:PATH = "$env:SystemRoot\System32;$env:SystemRoot"
    $env:QT_PLUGIN_PATH = $null
    $env:QT_QPA_PLATFORM = 'offscreen'
    $env:CAAC_CALCULATOR_SCREENSHOT_DIR = Join-Path $reports 'screenshots'
    & $testBinary -o "$(Join-Path $reports 'native-tests.xml'),junitxml"
    if ($LASTEXITCODE -ne 0) { throw 'Deployed calculator tests failed without SDK paths' }
    $smoke = Start-Process -FilePath $binary -ArgumentList '--smoke-test' -Wait -PassThru
    if ($smoke.ExitCode -ne 0) { throw 'Deployed calculator startup failed' }
} finally {
    $env:PATH = $savedPath
    $env:QT_QPA_PLATFORM = $savedPlatform
    $env:QT_PLUGIN_PATH = $savedPlugins
    $env:CAAC_CALCULATOR_SCREENSHOT_DIR = $null
    Remove-Item -LiteralPath $testBinary -Force
    Remove-Item -LiteralPath $testRuntime -Force
}

$manifest = Join-Path $output 'uninstall-files.nsh'
$deletions = @()
foreach ($file in Get-ChildItem -LiteralPath $stage -Recurse -File) {
    $relative = [IO.Path]::GetRelativePath($stage, $file.FullName).Replace('$', '$$')
    $deletions += 'Delete "$INSTDIR\' + $relative + '"'
}
foreach ($directory in Get-ChildItem -LiteralPath $stage -Recurse -Directory | Sort-Object { $_.FullName.Length } -Descending) {
    $relative = [IO.Path]::GetRelativePath($stage, $directory.FullName).Replace('$', '$$')
    $deletions += 'RMDir "$INSTDIR\' + $relative + '"'
}
$deletions | Set-Content -LiteralPath $manifest -Encoding utf8
$installer = Join-Path $output 'FeihuCalculator-1.0.0-Windows-x64-Setup.exe'
$compiler = Join-Path ${env:ProgramFiles(x86)} 'NSIS/makensis.exe'
& $compiler "/DPAYLOAD=$stage" "/DOUTPUT=$installer" "/DDELETE_MANIFEST=$manifest" (Join-Path $source 'installer.nsi')
if ($LASTEXITCODE -ne 0) { throw 'NSIS packaging failed' }

# Exercise the actual installer in a path containing spaces and Chinese characters.
$installed = Join-Path $env:RUNNER_TEMP 'calculator 安装验证/角度计算器'
$process = Start-Process -FilePath $installer -ArgumentList '/S', "/D=$installed" -Wait -PassThru
if ($process.ExitCode -ne 0) { throw 'Silent installation failed' }
$installedBinary = Join-Path $installed 'bin/CaacCalculator.exe'
$binaryHash = (Get-FileHash -LiteralPath $binary -Algorithm SHA256).Hash
try {
    if ((Get-FileHash -LiteralPath $installedBinary -Algorithm SHA256).Hash -cne $binaryHash) {
        throw 'Installed executable does not match deployed executable'
    }
    $shell = New-Object -ComObject WScript.Shell
    $shortcut = Join-Path ([Environment]::GetFolderPath('Desktop')) '飞虎角度计算器.lnk'
    if ($shell.CreateShortcut($shortcut).TargetPath -ne $installedBinary) { throw 'Desktop shortcut target mismatch' }
    $env:PATH = "$env:SystemRoot\System32;$env:SystemRoot"
    $env:QT_QPA_PLATFORM = 'offscreen'
    $smoke = Start-Process -FilePath $installedBinary -ArgumentList '--smoke-test' -Wait -PassThru
    if ($smoke.ExitCode -ne 0) { throw 'Installed calculator startup failed without SDK paths' }
    $env:QT_QPA_PLATFORM = $null
    $smoke = Start-Process -FilePath $installedBinary -ArgumentList '--smoke-test' -Wait -PassThru
    if ($smoke.ExitCode -ne 0) { throw 'Installed calculator native Windows startup failed' }
    Set-Content -LiteralPath (Join-Path $installed 'user-file.txt') -Value 'preserve unrelated user files' -Encoding ascii
} finally {
    $env:PATH = $savedPath
    $env:QT_QPA_PLATFORM = $savedPlatform
    $uninstaller = Join-Path $installed 'Uninstall.exe'
    if (Test-Path -LiteralPath $uninstaller) {
        $uninstall = Start-Process -FilePath $uninstaller -ArgumentList '/S', "_?=$installed" -Wait -PassThru
        if ($uninstall.ExitCode -ne 0) { throw 'Silent uninstallation failed' }
    }
}
if (Test-Path -LiteralPath $installedBinary) { throw 'Uninstall left the calculator executable behind' }
if (-not (Test-Path -LiteralPath (Join-Path $installed 'user-file.txt'))) { throw 'Uninstall removed an unrelated file' }
if (Test-Path -LiteralPath $shortcut) { throw 'Uninstall left the desktop shortcut behind' }

$installerHash = (Get-FileHash -LiteralPath $installer -Algorithm SHA256).Hash.ToLowerInvariant()
"$installerHash  $(Split-Path $installer -Leaf)" | Set-Content -LiteralPath (Join-Path $output 'SHA256SUMS.txt') -Encoding ascii
@{
    version = '1.0.0'
    sourceCommit = $sourceCommit
    qtVersion = $qtVersion
    installerSha256 = $installerHash
    executableSha256 = $binaryHash.ToLowerInvariant()
    sdkIndependentTestsPassed = $true
    chinesePathInstallPassed = $true
    desktopShortcutPassed = $true
    nativeWindowsStartupPassed = $true
    uninstallPassed = $true
    unrelatedFilePreserved = $true
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $output 'verification.json') -Encoding utf8
