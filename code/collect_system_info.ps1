$out = Join-Path $PSScriptRoot "results\system_info.txt"
$cpu = Get-CimInstance Win32_Processor | Select-Object -First 1 Name,NumberOfCores,NumberOfLogicalProcessors,MaxClockSpeed
$mem = Get-CimInstance Win32_ComputerSystem | Select-Object TotalPhysicalMemory
$os = Get-CimInstance Win32_OperatingSystem | Select-Object Caption,Version,OSArchitecture
"CPU: $($cpu.Name)" | Out-File -FilePath $out -Encoding utf8
"Cores: $($cpu.NumberOfCores)" | Out-File -FilePath $out -Append -Encoding utf8
"Logical processors: $($cpu.NumberOfLogicalProcessors)" | Out-File -FilePath $out -Append -Encoding utf8
"Max clock (MHz): $($cpu.MaxClockSpeed)" | Out-File -FilePath $out -Append -Encoding utf8
"Total memory (bytes): $($mem.TotalPhysicalMemory)" | Out-File -FilePath $out -Append -Encoding utf8
"OS: $($os.Caption)" | Out-File -FilePath $out -Append -Encoding utf8
"OS version: $($os.Version)" | Out-File -FilePath $out -Append -Encoding utf8
"Architecture: $($os.OSArchitecture)" | Out-File -FilePath $out -Append -Encoding utf8
Write-Host "System info written to $out"
