$ErrorActionPreference = 'Stop'
$fixture = Join-Path $env:RUNNER_TEMP 'caac-shortcut-fixture'
New-Item -ItemType Directory -Force -Path $fixture | Out-Null
$referencePath = Join-Path $fixture 'reference飞虎.lnk'
$shell = New-Object -ComObject WScript.Shell
$reference = $shell.CreateShortcut($referencePath)
$reference.TargetPath = Join-Path $env:WINDIR 'System32\notepad.exe'
$reference.Save()
$readReference = $shell.CreateShortcut($referencePath)
Write-Host "PowerShell version: $($PSVersionTable.PSVersion)"
Write-Host "Reference shortcut target: $($readReference.TargetPath)"
Write-Host "Reference shortcut type: $($readReference.GetType().FullName)"
if ([String]::IsNullOrWhiteSpace($readReference.TargetPath)) { throw 'Unable to read the reference shortcut' }

$compiler = Join-Path ${env:ProgramFiles(x86)} 'NSIS\makensis.exe'
if (-not (Test-Path $compiler)) {
    choco install nsis --no-progress -y
    if ($LASTEXITCODE -ne 0) { throw 'NSIS installation failed' }
}
$source = Join-Path $fixture 'source'
$build = Join-Path $fixture 'build'
New-Item -ItemType Directory -Force -Path "$source\.github", "$source\cmake\install" | Out-Null
Copy-Item "$env:GITHUB_WORKSPACE\cmake\install\ValidatePackageVersion.cmake" "$source\cmake\install\"
Set-Content "$source\.github\COPYING.md" 'fixture license'
Set-Content "$source\README.md" 'fixture readme'
$modulePath = "$env:GITHUB_WORKSPACE/cmake/install/CPack".Replace('\', '/')
$payload = "$env:WINDIR/System32/notepad.exe".Replace('\', '/')
$icon = (Get-ChildItem "$env:GITHUB_WORKSPACE/custom" -Filter '*.ico' -Recurse | Select-Object -First 1).FullName.Replace('\', '/')
$header = (Get-ChildItem "$env:GITHUB_WORKSPACE/custom" -Filter '*.bmp' -Recurse | Select-Object -First 1).FullName.Replace('\', '/')
@"
cmake_minimum_required(VERSION 3.25)
project(QGroundControl VERSION 5.0.0 DESCRIPTION "Shortcut fixture" LANGUAGES NONE)
list(APPEND CMAKE_MODULE_PATH "$modulePath")
set(QGC_ORG_NAME QGroundControl)
set(QGC_APP_DISPLAY_NAME "飞虎无人机地面站系统")
set(QGC_CAAC_BUILD TRUE)
set(QGC_WINDOWS_ICON_PATH "$icon")
set(QGC_WINDOWS_INSTALL_HEADER_PATH "$header")
install(FILES "$payload" DESTINATION bin RENAME QGroundControl.exe COMPONENT Runtime)
include(CreateCPackNSIS)
"@ | Set-Content "$source\CMakeLists.txt" -Encoding utf8
cmake -S $source -B $build -G "Visual Studio 17 2022"
if ($LASTEXITCODE -ne 0) { throw 'Fixture configure failed' }
cpack --config "$build\CPackConfig.cmake" -C Release -D "CPACK_NSIS_EXECUTABLE=$compiler"
if ($LASTEXITCODE -ne 0) { throw 'Fixture packaging failed' }
$installer = Get-ChildItem $build -Filter '*installer*.exe' | Select-Object -First 1
$installDir = Join-Path $fixture 'installed'
Start-Process $installer.FullName -ArgumentList '/S', "/D=$installDir" -Wait -NoNewWindow
$desktop = [Environment]::GetFolderPath([Environment+SpecialFolder]::CommonDesktopDirectory)
$desktopShortcut = Join-Path $desktop '飞虎无人机地面站系统.lnk'
$actual = $shell.CreateShortcut($desktopShortcut)
Write-Host "Actual shortcut target: $($actual.TargetPath)"
Write-Host "Actual shortcut working directory: $($actual.WorkingDirectory)"
Write-Host "Actual shortcut icon: $($actual.IconLocation)"
Write-Host "Actual shortcut type: $($actual.GetType().FullName)"
Copy-Item $desktopShortcut $fixture

$tokens = $null
$parseErrors = $null
$ast = [Management.Automation.Language.Parser]::ParseFile(
    "$env:GITHUB_WORKSPACE/deploy/windows/verify-installer.ps1", [ref]$tokens, [ref]$parseErrors)
$function = $ast.Find({param($node) $node -is [Management.Automation.Language.FunctionDefinitionAst] -and $node.Name -eq 'Assert-DesktopShortcut'}, $true)
. ([scriptblock]::Create($function.Extent.Text))
[void](Assert-DesktopShortcut (Join-Path $installDir 'bin\QGroundControl.exe'))
