# Package and exercise the issuer independently of the application and build SDKs.
param(
    [Parameter(Mandatory)][string]$Binary,
    [Parameter(Mandatory)][string]$QtRoot,
    [Parameter(Mandatory)][string]$OutputDirectory
)
$ErrorActionPreference = 'Stop'
$output = (New-Item -ItemType Directory -Path $OutputDirectory).FullName
$stage = (New-Item -ItemType Directory -Path (Join-Path $output 'CaacLicenseTool-Windows-x64')).FullName
$tool = Join-Path $stage 'CaacLicenseTool.exe'
Copy-Item -LiteralPath $Binary -Destination $tool
& (Join-Path $QtRoot 'bin/windeployqt.exe') --release --no-translations --no-opengl-sw --no-compiler-runtime `
    --no-system-d3d-compiler --dir $stage $tool
if ($LASTEXITCODE -ne 0) { throw 'windeployqt failed for CaacLicenseTool' }

# App-local CRT DLLs let the tool work without an installed Visual Studio runtime.
$redist = Join-Path $env:VCToolsRedistDir 'x64/Microsoft.VC143.CRT'
if (-not (Test-Path -LiteralPath $redist)) { throw "MSVC runtime directory missing: $redist" }
Copy-Item -Path (Join-Path $redist '*.dll') -Destination $stage
Copy-Item -LiteralPath 'custom/activation/public-key.hex' -Destination $stage
Copy-Item -LiteralPath 'custom/activation/windows-tool-README.txt' -Destination (Join-Path $stage '使用说明.txt')
$envBefore = $env:PATH
$scratch = (New-Item -ItemType Directory -Path (Join-Path $output 'smoke-private-temporary')).FullName
try {
    $env:PATH = "$env:SystemRoot\System32;$env:SystemRoot"
    $privateKey = Join-Path $scratch 'test-private.hex'
    $publicKey = Join-Path $scratch 'test-public.hex'
    & $tool keygen --private-key $privateKey --public-key $publicKey | Out-Null
    if ($LASTEXITCODE -ne 0) { throw 'Packaged keygen failed without SDK paths' }
    $activation = Join-Path $scratch 'test-activation.txt'
    & $tool issue --private-key $privateKey --machine 'CAAC-01234567-89ABCDEF-01234567-89ABCDEF' --output $activation | Out-Null
    if ($LASTEXITCODE -ne 0) { throw 'Packaged issue failed without SDK paths' }
    $code = (Get-Content -LiteralPath $activation -Raw).Trim()
    if ($code -cnotmatch '^CAAC1\.[A-Za-z0-9_-]{107}$') { throw 'Invalid issued activation code format' }
    & $tool issue --private-key $privateKey --machine 'CAAC-01234567-89ABCDEF-01234567-89ABCDEF' --output $activation 2>$null | Out-Null
    if ($LASTEXITCODE -eq 0) { throw 'Issuer overwrote an existing activation file' }
    if ((Get-Content -LiteralPath $activation -Raw).Trim() -cne $code) { throw 'Activation output changed' }
    & $tool issue --private-key $privateKey --machine 'invalid-machine-code' 2>$null | Out-Null
    if ($LASTEXITCODE -eq 0) { throw 'Issuer accepted an invalid machine code' }
    $global:LASTEXITCODE = 0
} finally {
    $env:PATH = $envBefore
    Remove-Item -LiteralPath $scratch -Recurse -Force
}
$unexpected = @(Get-ChildItem -LiteralPath $stage -Recurse -File | Where-Object {
    $_.Name -match 'private|\.license$' -or ($_.Extension -eq '.hex' -and $_.Name -ne 'public-key.hex')
})
if ($unexpected.Count -ne 0) { throw 'Unexpected key or license material in the issuer bundle' }
@{
    sourceCommit = $env:GITHUB_SHA
    platform = 'windows-x64'
    sdkIndependentSmokePassed = $true
    publicKeySha256 = (Get-FileHash -LiteralPath (Join-Path $stage 'public-key.hex') -Algorithm SHA256).Hash.ToLowerInvariant()
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $stage 'build.json') -Encoding utf8
$archive = Join-Path $output 'CaacLicenseTool-Windows-x64.zip'
Compress-Archive -LiteralPath $stage -DestinationPath $archive
$checksum = (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
"$checksum  CaacLicenseTool-Windows-x64.zip" | Set-Content -LiteralPath "$archive.sha256" -Encoding ascii
Write-Host "Verified issuer bundle: $archive"
