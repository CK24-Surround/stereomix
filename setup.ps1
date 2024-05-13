param([switch]$ShowProgress)

# Powershell 옛날 버전은 프로그레스바 사용 시 다운로드 속도가 느려지는 버그가 있음
if ($ShowProgress -eq $true) {
    $ProgressPreference = "Continue"
}
else {
    $ProgressPreference = "SilentlyContinue"
}

$TurboLinkZipUri = "https://github.com/thejinchao/turbolink-libraries/releases/download/v1.3.1/turbolink-libraries.ue54.zip"
$TurboLinkZipName = "turbolink-lib.zip"

Write-Output "Downloading TurboLink libraries..."
Invoke-WebRequest -URI $TurboLinkZipUri -OutFile $TurboLinkZipName
Write-Output "Unzip TurboLink libraries..."
Expand-Archive -Path $TurboLinkZipName -Force -DestinationPath ./StereoMix/Plugins/TurboLink/Source/ThirdParty
Write-Output "Removing zip file..."
Remove-Item -Path $TurboLinkZipName
Write-Output "Successfully setup TurboLink plugin."