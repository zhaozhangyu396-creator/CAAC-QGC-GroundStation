$ErrorActionPreference = 'Stop'
. "$env:GITHUB_WORKSPACE/deploy/windows/read-shortcut.ps1"
$fixture = Join-Path $env:RUNNER_TEMP 'caac-shortcut-fixture'
New-Item -ItemType Directory -Force -Path $fixture | Out-Null
$referencePath = Join-Path $fixture 'reference.lnk'
$shell = New-Object -ComObject WScript.Shell
$reference = $shell.CreateShortcut($referencePath)
$reference.TargetPath = Join-Path $env:WINDIR 'System32\notepad.exe'
$reference.Save()
$readReference = $shell.CreateShortcut($referencePath)
Write-Host "PowerShell version: $($PSVersionTable.PSVersion)"
Write-Host "Reference shortcut target: $($readReference.TargetPath)"
Write-Host "Reference shortcut type: $($readReference.GetType().FullName)"
if ([String]::IsNullOrWhiteSpace($readReference.TargetPath)) { throw 'Unable to read the reference shortcut' }
$unicodeReference = Join-Path $fixture 'reference飞虎.lnk'
Copy-Item $referencePath $unicodeReference
$nativeReference = [QgcWindowsShortcut]::Read($unicodeReference)
if ($nativeReference.TargetPath -ne $readReference.TargetPath) { throw 'Native Unicode shortcut reader mismatch' }
Write-Host "Native Unicode reference target: $($nativeReference.TargetPath)"

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
$payloadSha256 = (Get-FileHash -LiteralPath $payload -Algorithm SHA256).Hash.ToLowerInvariant()
$companyIcon = Join-Path $env:GITHUB_WORKSPACE 'custom\assets\company-logo.ico'
$legacyIcon = Join-Path $env:GITHUB_WORKSPACE 'resources\icons\qgroundcontrol.ico'
$companyIconSha256 = (Get-FileHash -LiteralPath $companyIcon -Algorithm SHA256).Hash.ToLowerInvariant()
$legacyIconSha256 = (Get-FileHash -LiteralPath $legacyIcon -Algorithm SHA256).Hash.ToLowerInvariant()
if ($companyIconSha256 -eq $legacyIconSha256) { throw 'The upgrade fixture needs distinct company and legacy icons' }
$header = (Get-ChildItem "$env:GITHUB_WORKSPACE/custom" -Filter '*.bmp' -Recurse | Select-Object -First 1).FullName.Replace('\', '\\')
@"
cmake_minimum_required(VERSION 3.25)
project(QGroundControl VERSION 5.0.0 DESCRIPTION "Shortcut fixture" LANGUAGES NONE)
list(APPEND CMAKE_MODULE_PATH "$modulePath")
set(QGC_ORG_NAME QGroundControl)
set(QGC_APP_DISPLAY_NAME "飞虎无人机地面站系统")
set(QGC_CAAC_BUILD TRUE)
set(QGC_WINDOWS_INSTALL_HEADER_PATH "$header")
install(FILES "$payload" DESTINATION bin RENAME QGroundControl.exe COMPONENT Runtime)
include(CreateCPackNSIS)
"@ | Set-Content "$source\CMakeLists.txt" -Encoding utf8
$installDir = Join-Path $fixture 'installed'
$binary = Join-Path $installDir 'bin\QGroundControl.exe'
$installedCompanyIcon = Join-Path $installDir "bin\QGroundControl-icon-$companyIconSha256.ico"
$installedLegacyIcon = Join-Path $installDir "bin\QGroundControl-icon-$legacyIconSha256.ico"
$desktop = [Environment]::GetFolderPath([Environment+SpecialFolder]::CommonDesktopDirectory)
$desktopShortcut = Join-Path $desktop '飞虎无人机地面站系统.lnk'
$tokens = $null
$parseErrors = $null
$ast = [Management.Automation.Language.Parser]::ParseFile(
    "$env:GITHUB_WORKSPACE/deploy/windows/verify-installer.ps1", [ref]$tokens, [ref]$parseErrors)
if ($parseErrors.Count) { throw "Shortcut validator parse failed: $parseErrors" }
foreach ($name in @('Assert-ApplicationShortcut', 'Assert-DesktopShortcut')) {
    $function = $ast.Find({param($node) $node -is [Management.Automation.Language.FunctionDefinitionAst] -and $node.Name -eq $name}, $true)
    if (-not $function) { throw "Shortcut validator function missing: $name" }
    . ([scriptblock]::Create($function.Extent.Text))
}

function Assert-ShortcutRejected([string]$Description, [string]$ExpectedMessage, [scriptblock]$Check) {
    $rejected = $false
    try {
        & $Check | Out-Null
    } catch {
        if ($_.Exception.Message -notlike $ExpectedMessage) { throw }
        $rejected = $true
    }
    if (-not $rejected) { throw "The validator accepted $Description" }
    Write-Host "Rejected $Description as expected"
}

$uninstallKey = 'HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall\QGroundControl'
$shortcuts = @()
$mutation = Join-Path $fixture 'mutation.lnk'
try {
    foreach ($iconSource in @($legacyIcon, $companyIcon)) {
        cmake -S $source -B $build -G "Visual Studio 17 2022" "-DQGC_WINDOWS_ICON_PATH:FILEPATH=$($iconSource.Replace('\', '/'))"
        if ($LASTEXITCODE -ne 0) { throw 'Fixture configure failed' }
        cpack --config "$build\CPackConfig.cmake" -C Release -D "CPACK_NSIS_EXECUTABLE=$compiler"
        if ($LASTEXITCODE -ne 0) {
            Get-ChildItem "$build/_CPack_Packages" -Filter NSISOutput.log -Recurse | ForEach-Object { Get-Content $_.FullName }
            throw 'Fixture packaging failed'
        }
        $installer = Get-ChildItem $build -Filter '*installer*.exe' | Select-Object -First 1
        $installation = Start-Process $installer.FullName -ArgumentList '/S', "/D=$installDir" -Wait -NoNewWindow -PassThru
        if ($installation.ExitCode -ne 0) { throw "Fixture installation failed: $($installation.ExitCode)" }
        if (-not (Test-Path -LiteralPath $binary -PathType Leaf)) {
            throw "Fixture executable missing after installation: $binary"
        }
        if ((Get-FileHash -LiteralPath $binary -Algorithm SHA256).Hash.ToLowerInvariant() -cne $payloadSha256) {
            throw "Fixture executable checksum mismatch after installation: $binary"
        }
        $startMenu = (Get-ItemProperty -LiteralPath $uninstallKey).StartMenu
        $startMenuDir = Join-Path (Join-Path $env:ProgramData 'Microsoft\Windows\Start Menu\Programs') $startMenu
        $shortcuts = @(
            @{ Path = $desktopShortcut; Arguments = '' },
            @{ Path = (Join-Path $startMenuDir '飞虎无人机地面站系统.lnk'); Arguments = '' },
            @{ Path = (Join-Path $startMenuDir '飞虎无人机地面站系统 (显卡兼容模式).lnk'); Arguments = '-swrast' }
        )
        $expectedSha256 = (Get-FileHash -LiteralPath $iconSource -Algorithm SHA256).Hash.ToLowerInvariant()
        [void](Assert-DesktopShortcut $binary $expectedSha256)
        foreach ($shortcut in $shortcuts) {
            [void](Assert-ApplicationShortcut $shortcut.Path $binary $shortcut.Arguments $expectedSha256)
        }
        if ($iconSource -eq $legacyIcon) {
            # Seed the fixed executable icon location used by older installed versions before upgrading in place.
            foreach ($shortcut in $shortcuts) {
                Copy-Item -LiteralPath $shortcut.Path -Destination $mutation -Force
                $stale = $shell.CreateShortcut($mutation)
                $stale.IconLocation = "$binary,0"
                $stale.Save()
                Copy-Item -LiteralPath $mutation -Destination $shortcut.Path -Force
            }
            Assert-ShortcutRejected 'the legacy executable icon location' 'Shortcut icon location mismatch:*' {
                Assert-DesktopShortcut $binary $legacyIconSha256
            }
        } elseif (Test-Path -LiteralPath $installedLegacyIcon) {
            throw 'The previous content-addressed icon remains after the same-directory upgrade'
        }
    }
    Write-Host 'Same-directory upgrade replaced all legacy icon locations with the new company icon hash'
    $backup = Join-Path $fixture 'valid-company.lnk'
    Copy-Item -LiteralPath $desktopShortcut -Destination $backup -Force
    $cases = @(
        @{ Description = 'a shortcut to the wrong executable'; Target = $readReference.TargetPath;
           Icon = "$installedCompanyIcon,0"; Error = 'Shortcut target mismatch:*' },
        @{ Description = 'the old executable icon location'; Target = $binary;
           Icon = "$binary,0"; Error = 'Shortcut icon location mismatch:*' },
        @{ Description = 'a different company icon'; Target = $binary;
           Icon = "$legacyIcon,0"; Error = 'Shortcut icon location mismatch:*' }
    )
    try {
        foreach ($case in $cases) {
            Copy-Item -LiteralPath $backup -Destination $mutation -Force
            $invalid = $shell.CreateShortcut($mutation)
            $invalid.TargetPath = $case.Target
            $invalid.IconLocation = $case.Icon
            $invalid.Save()
            Copy-Item -LiteralPath $mutation -Destination $desktopShortcut -Force
            Assert-ShortcutRejected $case.Description $case.Error {
                Assert-DesktopShortcut $binary $companyIconSha256
            }
        }
    } finally {
        Copy-Item -LiteralPath $backup -Destination $desktopShortcut -Force
    }
    try {
        Copy-Item -LiteralPath $legacyIcon -Destination $installedCompanyIcon -Force
        Assert-ShortcutRejected 'incorrect icon bytes under the expected company icon filename' 'Company shortcut icon checksum mismatch:*' {
            Assert-DesktopShortcut $binary $companyIconSha256
        }
    } finally {
        Copy-Item -LiteralPath $companyIcon -Destination $installedCompanyIcon -Force
    }
    foreach ($shortcut in $shortcuts) {
        [void](Assert-ApplicationShortcut $shortcut.Path $binary $shortcut.Arguments $companyIconSha256)
    }
} finally {
    $uninstaller = Join-Path $installDir 'QGroundControl-Uninstall.exe'
    if (Test-Path -LiteralPath $uninstaller) {
        $removal = Start-Process $uninstaller -ArgumentList "/S -LEAVE_DATA=1 _?=$installDir" -Wait -NoNewWindow -PassThru
        if ($removal.ExitCode -ne 0) { throw "Fixture uninstall failed: $($removal.ExitCode)" }
    }
    foreach ($path in @($binary, $installedCompanyIcon, $installedLegacyIcon) + @($shortcuts | ForEach-Object { $_.Path })) {
        if (Test-Path -LiteralPath $path) { throw "Fixture install artifact remains after uninstall: $path" }
    }
    if (Test-Path -LiteralPath $uninstallKey) { throw 'Fixture uninstall registry key remains after cleanup' }
}
Write-Host 'Shortcut icon upgrade fixture uninstalled without removing business application data'
