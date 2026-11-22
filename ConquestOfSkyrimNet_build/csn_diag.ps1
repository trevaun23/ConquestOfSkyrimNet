# Conquest of SkyrimNet - find which script makes the Papyrus compiler hang or fail.
# Compiles each script in the staged build on its own, 120 s limit each, results in csn_diag_log.txt.
$ErrorActionPreference = "Continue"
$here  = Split-Path -Parent $MyInvocation.MyCommand.Path
$log   = Join-Path $here "csn_diag_log.txt"
$game  = "C:\Program Files (x86)\The.Elder.Scrolls.V.Skyrim.Special.Edition.v1.6.1170\game"
$data  = Join-Path $game "Data"
$pc    = Join-Path $game "Papyrus Compiler\PapyrusCompiler.exe"
$stage = Join-Path $env:USERPROFILE "ffbuild\csnmod"
$flg   = Join-Path $data "source\Scripts\TESV_Papyrus_Flags.flg"
$imp   = "$stage\Scripts\Source;$here\stubs;$data\Scripts\Source;$data\source\Scripts"
$out   = Join-Path $env:USERPROFILE "ffbuild\diagout"
New-Item -ItemType Directory -Force -Path $out | Out-Null
"Stopping any stuck PapyrusCompiler..." | Out-File $log
Get-Process PapyrusCompiler -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep 2
Expand-Archive -Force -LiteralPath (Join-Path $here "CSN_Dev.zip") -DestinationPath $stage
$src = Join-Path $stage "Source\Scripts"
if (!(Test-Path $src)) { "Staged sources not found at $src - run build_all.bat once first." | Out-File $log -Append; exit 1 }
$files = Get-ChildItem $src -Filter "*.psc" | Sort-Object Name
foreach ($f in $files) {
  $one = Join-Path $out ($f.BaseName + ".txt")
  $p = Start-Process -FilePath $pc -ArgumentList "`"$($f.FullName)`" -f=`"$flg`" -i=`"$imp`" -o=`"$out`"" -NoNewWindow -PassThru -RedirectStandardOutput $one -RedirectStandardError ($one + ".err")
  if (!$p.WaitForExit(120000)) {
    $p.Kill()
    "HUNG    $($f.Name)" | Out-File $log -Append
  } else {
    $txt = (Get-Content $one -Raw) + (Get-Content ($one + ".err") -Raw)
    if ($txt -match "Compilation succeeded" -and $txt -notmatch "failed") { "ok      $($f.Name)" | Out-File $log -Append }
    else { "FAILED  $($f.Name)" | Out-File $log -Append; ($txt -split "`n" | Select-String -Pattern "error|\(\d+,\d+\)" | Select-Object -First 25) | Out-File $log -Append }
  }
}
"done" | Out-File $log -Append
