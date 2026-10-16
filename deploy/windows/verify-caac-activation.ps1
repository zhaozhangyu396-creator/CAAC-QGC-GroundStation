# Verify the production activation gate without a license or an authorization bypass.
param([Parameter(Mandatory)][string]$Binary, [Parameter(Mandatory)][string]$Report)
$ErrorActionPreference = 'Stop'
$binaryPath = (Get-Item -LiteralPath $Binary).FullName
$licensePath = Join-Path $env:LOCALAPPDATA 'CAAC/GroundStation/activation.license'
if (Test-Path -LiteralPath $licensePath) {
    throw 'Activation verification requires an unlicensed CI runner'
}
$logBase = [IO.Path]::ChangeExtension($Report, $null)
$process = Start-Process -FilePath $binaryPath -WorkingDirectory (Split-Path $binaryPath) `
    -RedirectStandardOutput "$logBase.stdout.log" -RedirectStandardError "$logBase.stderr.log" -PassThru
try {
    $deadline = [DateTime]::UtcNow.AddSeconds(60)
    $visibleSince = $null
    while ([DateTime]::UtcNow -lt $deadline) {
        $process.Refresh()
        if ($process.HasExited) { throw "Application exited before activation verification: $($process.ExitCode)" }
        $title = $process.MainWindowTitle
        if ($title -in @('地面站激活', 'Ground station activation')) {
            if ($null -eq $visibleSince) { $visibleSince = [DateTime]::UtcNow }
            if (([DateTime]::UtcNow - $visibleSince).TotalSeconds -ge 3) { break }
        } else {
            $visibleSince = $null
        }
        Start-Sleep -Milliseconds 100
    }
    if ($null -eq $visibleSince -or ([DateTime]::UtcNow - $visibleSince).TotalSeconds -lt 3) {
        throw "Activation window did not remain visible; last title: $title"
    }
    if (-not $process.CloseMainWindow() -or -not $process.WaitForExit(15000)) {
        throw 'Activation window did not close cleanly'
    }
    if ($process.ExitCode -ne 0) { throw "Activation exit code: $($process.ExitCode)" }
    if (Test-Path -LiteralPath $licensePath) { throw 'Closing activation created an unexpected license' }
    @{
        binary = $binaryPath
        sha256 = (Get-FileHash -LiteralPath $binaryPath -Algorithm SHA256).Hash.ToLowerInvariant()
        title = $title
        activationWindowVerified = $true
        exitCode = $process.ExitCode
        sourceCommit = $env:GITHUB_SHA
    } | ConvertTo-Json | Set-Content -LiteralPath $Report -Encoding utf8
    Write-Host "Verified unlicensed activation window and clean exit: $binaryPath"
} finally {
    if (-not $process.HasExited) { Stop-Process -Id $process.Id -Force }
    foreach ($log in @("$logBase.stdout.log", "$logBase.stderr.log")) {
        if (Test-Path -LiteralPath $log) { Get-Content -LiteralPath $log -Tail 60 }
    }
}
