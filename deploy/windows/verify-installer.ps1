# Exercise installation, upgrade preservation, and uninstallation on a CI runner.
param([Parameter(Mandatory)][ValidateSet('Install', 'Uninstall')][string]$Phase)
$ErrorActionPreference = 'Stop'
. "$PSScriptRoot/read-shortcut.ps1"

function Assert-ApplicationShortcut([string]$ShortcutPath, [string]$Binary, [string]$Arguments, [string]$IconSha256) {
    if (-not (Test-Path -LiteralPath $Binary -PathType Leaf)) {
      throw "Shortcut executable missing: $Binary"
    }
    if ($IconSha256 -cnotmatch '^[a-f0-9]{64}$') {
      throw "Invalid company icon SHA-256: $IconSha256"
    }
    $iconPath = Join-Path (Split-Path $Binary) "QGroundControl-icon-$IconSha256.ico"
    if (-not (Test-Path -LiteralPath $iconPath -PathType Leaf)) {
      throw "Company shortcut icon missing: $iconPath"
    }
    if ((Get-FileHash -LiteralPath $iconPath -Algorithm SHA256).Hash.ToLowerInvariant() -cne $IconSha256) {
      throw "Company shortcut icon checksum mismatch: $iconPath"
    }
    if (-not (Test-Path -LiteralPath $ShortcutPath -PathType Leaf)) {
      throw "Application shortcut not found: $ShortcutPath"
    }
    $shortcut = [QgcWindowsShortcut]::Read($ShortcutPath)
    if (-not [StringComparer]::OrdinalIgnoreCase.Equals($shortcut.TargetPath, $Binary)) {
      throw "Shortcut target mismatch: $($shortcut.TargetPath)"
    }
    if ($shortcut.Arguments -ne $Arguments) {
      throw "Unexpected shortcut arguments: $($shortcut.Arguments)"
    }
    if (-not [StringComparer]::OrdinalIgnoreCase.Equals($shortcut.WorkingDirectory, (Split-Path $Binary))) {
      throw "Shortcut working directory mismatch: $($shortcut.WorkingDirectory)"
    }
    if (-not [StringComparer]::OrdinalIgnoreCase.Equals($shortcut.IconLocation, "$iconPath,0")) {
      throw "Shortcut icon location mismatch: $($shortcut.IconLocation)"
    }
    Write-Host "Verified application shortcut and company icon SHA-256: $ShortcutPath"
    return $ShortcutPath
}

function Assert-DesktopShortcut([string]$Binary, [string]$IconSha256) {
    $desktop = [Environment]::GetFolderPath([Environment+SpecialFolder]::CommonDesktopDirectory)
    if ([String]::IsNullOrWhiteSpace($desktop)) {
      throw "Unable to resolve the public desktop directory"
    }
    $shortcutPath = Join-Path $desktop "飞虎无人机地面站系统.lnk"
    if (-not (Test-Path -LiteralPath $shortcutPath -PathType Leaf)) {
      throw "Desktop shortcut not found: $shortcutPath"
    }
    return (Assert-ApplicationShortcut $shortcutPath $Binary "" $IconSha256)
}

if ($Phase -eq 'Install') {
    if ($env:CAAC_CHINESE_INSTALLER -eq "true") {
      $sourceIcon = Join-Path $PSScriptRoot '..\..\custom\assets\company-logo.ico'
      $companyIconSha256 = (Get-FileHash -LiteralPath $sourceIcon -Algorithm SHA256).Hash.ToLowerInvariant()
    }
    $installerExe = Get-Item -LiteralPath $env:INSTALLER_PATH
    $oldInstallDir = $env:OLD_INSTALL_DIR
    $installDir = $env:INSTALL_DIR
    Write-Host "Installing $($installerExe.Name) to $oldInstallDir"
    Start-Process -FilePath $installerExe.FullName -ArgumentList "/S", "/D=$oldInstallDir" -Wait -NoNewWindow
    $oldBinary = Join-Path $oldInstallDir "bin\QGroundControl.exe"
    if (-not (Test-Path $oldBinary)) {
      Write-Error "QGroundControl.exe not found at $oldBinary after initial install"
      exit 1
    }
    if ($env:CAAC_CHINESE_INSTALLER -eq "true") {
      [void](Assert-DesktopShortcut $oldBinary $companyIconSha256)
    }
    $settingsRoot = [Environment]::GetFolderPath([Environment+SpecialFolder]::ApplicationData)
    if ([String]::IsNullOrWhiteSpace($settingsRoot)) {
      Write-Error "Unable to resolve the current user's roaming application data directory"
      exit 1
    }
    $settingsDir = Join-Path $settingsRoot "QGroundControl"
    $settingsMarker = Join-Path $settingsDir "installer-upgrade-test.txt"
    New-Item -ItemType Directory -Force -Path $settingsDir | Out-Null
    Set-Content -Path $settingsMarker -Value "preserve during upgrade"
    Write-Host "Upgrading into a different directory: $installDir"
    Start-Process -FilePath $installerExe.FullName -ArgumentList "/S", "/D=$installDir" -Wait -NoNewWindow
    if (Test-Path $oldBinary) {
      Write-Error "Previous installation was not removed from $oldInstallDir"
      exit 1
    }
    if (-not (Test-Path $settingsMarker)) {
      Write-Error "Application data was removed during upgrade"
      exit 1
    }
    $binary = Join-Path $installDir "bin\QGroundControl.exe"
    if (-not (Test-Path $binary)) {
      Write-Error "QGroundControl.exe not found at $binary after upgrade"
      Get-ChildItem -Path $installDir -Recurse | Select-Object FullName
      exit 1
    }
    if ($env:CAAC_CHINESE_INSTALLER -eq "true") {
      $desktopShortcut = Assert-DesktopShortcut $binary $companyIconSha256
      "QGC_INSTALL_TEST_DESKTOP_SHORTCUT=$desktopShortcut" | Out-File -FilePath $env:GITHUB_ENV -Append -Encoding utf8
      $installedIcon = Join-Path (Split-Path $binary) "QGroundControl-icon-$companyIconSha256.ico"
      "QGC_INSTALL_TEST_ICON=$installedIcon" | Out-File -FilePath $env:GITHUB_ENV -Append -Encoding utf8
      $versionInfo = (Get-Item -LiteralPath $binary).VersionInfo
      if ($versionInfo.ProductName -ne "飞虎无人机地面站系统" -or $versionInfo.FileDescription -ne "飞虎无人机地面站系统") {
        throw "Installed executable branding does not match the company application name"
      }
    }
    $expectGstreamer = $env:EXPECT_GSTREAMER -eq "true"
    $pluginDir = Join-Path $installDir "lib\gstreamer-1.0"
    if (-not $expectGstreamer) {
      Write-Host "GStreamer disabled for this build; skipping plugin verification"
    } elseif (Test-Path $pluginDir) {
      $pluginCount = (Get-ChildItem -Path $pluginDir -Filter "*.dll" | Measure-Object).Count
      Write-Host "GStreamer plugins found: $pluginCount"
      if ($pluginCount -eq 0) {
        Write-Error "No GStreamer plugin DLLs found in $pluginDir"
        exit 1
      }
    } else {
      Write-Error "GStreamer plugin directory not found: $pluginDir"
      exit 1
    }
    $uninstallKey = "HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall\QGroundControl"
    $werKey = "HKLM:\Software\Microsoft\Windows\Windows Error Reporting\LocalDumps\QGroundControl.exe"
    if (-not (Test-Path -LiteralPath $uninstallKey)) {
      Write-Error "Uninstall registry key not found: $uninstallKey"
      exit 1
    }
    $uninstallProperties = Get-ItemProperty -LiteralPath $uninstallKey
    $actualInstallDir = [IO.Path]::GetFullPath($uninstallProperties.InstallLocation).TrimEnd([IO.Path]::DirectorySeparatorChar)
    $expectedInstallDir = [IO.Path]::GetFullPath($installDir).TrimEnd([IO.Path]::DirectorySeparatorChar)
    if (-not [StringComparer]::OrdinalIgnoreCase.Equals($actualInstallDir, $expectedInstallDir)) {
      Write-Error "InstallLocation mismatch: expected $expectedInstallDir, got $actualInstallDir"
      exit 1
    }
    if (-not (Test-Path -LiteralPath $werKey)) {
      Write-Error "Windows Error Reporting registry key not found: $werKey"
      exit 1
    }
    $werProperties = Get-ItemProperty -LiteralPath $werKey
    if ($werProperties.DumpCount -ne 5 -or $werProperties.DumpType -ne 1) {
      Write-Error "Unexpected WER dump policy: count=$($werProperties.DumpCount), type=$($werProperties.DumpType)"
      exit 1
    }
    $werRegistryKey = [Microsoft.Win32.Registry]::LocalMachine.OpenSubKey(
      "Software\Microsoft\Windows\Windows Error Reporting\LocalDumps\QGroundControl.exe"
    )
    if ($null -eq $werRegistryKey) {
      Write-Error "Unable to open Windows Error Reporting registry key"
      exit 1
    }
    try {
      $dumpFolder = $werRegistryKey.GetValue(
        "DumpFolder", $null, [Microsoft.Win32.RegistryValueOptions]::DoNotExpandEnvironmentNames
      )
    } finally {
      $werRegistryKey.Dispose()
    }
    if ($dumpFolder -ne "%LOCALAPPDATA%\QGCCrashDumps") {
      Write-Error "Unexpected WER dump folder: $dumpFolder"
      exit 1
    }
    $startMenuRoot = Join-Path $env:ProgramData "Microsoft\Windows\Start Menu\Programs"
    $startMenuDir = Join-Path $startMenuRoot $uninstallProperties.StartMenu
    $safeModeName = "QGroundControl (GPU Safe Mode).lnk"
    if ($env:CAAC_CHINESE_INSTALLER -eq "true") {
      $safeModeName = "飞虎无人机地面站系统 (显卡兼容模式).lnk"
    }
    $shortcuts = @(
      (Join-Path $startMenuDir $(if ($env:CAAC_CHINESE_INSTALLER -eq "true") { "飞虎无人机地面站系统.lnk" } else { "QGroundControl.lnk" })),
      (Join-Path $startMenuDir $safeModeName)
    )
    foreach ($shortcut in $shortcuts) {
      if (-not (Test-Path -LiteralPath $shortcut -PathType Leaf)) {
        Write-Error "Start Menu shortcut not found: $shortcut"
        exit 1
      }
      if ($env:CAAC_CHINESE_INSTALLER -eq "true") {
        $arguments = if ((Split-Path $shortcut -Leaf) -eq $safeModeName) { "-swrast" } else { "" }
        [void](Assert-ApplicationShortcut $shortcut $binary $arguments $companyIconSha256)
      }
    }
    "QGC_INSTALL_TEST_DIR=$installDir" | Out-File -FilePath $env:GITHUB_ENV -Append -Encoding utf8
    "QGC_INSTALL_TEST_BINARY=$binary" | Out-File -FilePath $env:GITHUB_ENV -Append -Encoding utf8
    "QGC_INSTALL_TEST_SETTINGS=$settingsDir" | Out-File -FilePath $env:GITHUB_ENV -Append -Encoding utf8
    "QGC_INSTALL_TEST_UNINSTALL_KEY=$uninstallKey" | Out-File -FilePath $env:GITHUB_ENV -Append -Encoding utf8
    "QGC_INSTALL_TEST_WER_KEY=$werKey" | Out-File -FilePath $env:GITHUB_ENV -Append -Encoding utf8
    "QGC_INSTALL_TEST_START_MENU=$startMenuDir" | Out-File -FilePath $env:GITHUB_ENV -Append -Encoding utf8
}

if ($Phase -eq 'Uninstall') {
    $uninstaller = Get-ChildItem -Path $env:QGC_INSTALL_TEST_DIR -Filter "*Uninstall*.exe" -Recurse | Select-Object -First 1
    if (-not $uninstaller) {
      Write-Error "Uninstaller not found in $env:QGC_INSTALL_TEST_DIR"
      exit 1
    }
    # _?= keeps the self-copying NSIS uninstaller in place so -Wait is meaningful.
    Start-Process -FilePath $uninstaller.FullName -ArgumentList "/S _?=$($uninstaller.DirectoryName)" -Wait -NoNewWindow
    if (Test-Path $env:QGC_INSTALL_TEST_BINARY) {
      Write-Error "Installed executable remains after uninstall"
      exit 1
    }
    if ($env:QGC_INSTALL_TEST_ICON -and (Test-Path -LiteralPath $env:QGC_INSTALL_TEST_ICON)) {
      throw "Company shortcut icon remains after uninstall"
    }
    if (Test-Path $env:QGC_INSTALL_TEST_SETTINGS) {
      Write-Error "Application data remains after a normal uninstall"
      exit 1
    }
    if (Test-Path -LiteralPath $env:QGC_INSTALL_TEST_UNINSTALL_KEY) {
      Write-Error "Uninstall registry key remains after uninstall"
      exit 1
    }
    if (Test-Path -LiteralPath $env:QGC_INSTALL_TEST_WER_KEY) {
      Write-Error "Windows Error Reporting registry key remains after uninstall"
      exit 1
    }
    if ($env:QGC_INSTALL_TEST_DESKTOP_SHORTCUT -and (Test-Path -LiteralPath $env:QGC_INSTALL_TEST_DESKTOP_SHORTCUT)) {
      throw "Desktop shortcut remains after uninstall"
    }
    if (Test-Path -LiteralPath $env:QGC_INSTALL_TEST_START_MENU) {
      Write-Error "Start Menu directory remains after uninstall"
      exit 1
    }
}
