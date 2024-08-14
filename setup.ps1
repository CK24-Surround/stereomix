param([switch]$ShowProgress)

$RootDir = $PSScriptRoot

# Powershell 옛날 버전은 프로그레스바 사용 시 다운로드 속도가 느려지는 버그가 있음
if ($ShowProgress -eq $true) {
    $ProgressPreference = "Continue"
}
else {
    $ProgressPreference = "SilentlyContinue"
}

# 구글 드라이브 다운로드 링크는
# https://drive.usercontent.google.com/download?id={drive_id}&export=download&confirm=t
# drive_id 항목을 공유 링크의 id로 변경하면 됨
function Get-GoogleDriveDownloadLink {
    param (
        [string] $Id
    )

    return "https://drive.usercontent.google.com/download?id=$Id&export=download&confirm=t"
}

function Setup {
    param (
        $ProcessName,
        $ZipUri,
        [scriptblock] $SubProcess
    )
    
    Write-Output "Start setup $ProcessName"

    $TempName = "." + $ProcessName + "_Temp"

    $TempFilePath = [System.IO.Path]::Combine($RootDir, $TempName + ".zip") 
    $TempDirPath = [System.IO.Path]::Combine($RootDir, $TempName)

    Write-Output "Downloading zip file..."
    Invoke-WebRequest -URI $ZipUri -OutFile $TempFilePath # {ProcessName}_Temp.zip

    Write-Output "Extracting files..."
    Expand-Archive -Path $TempFilePath -Force -DestinationPath $TempDirPath

    Write-Output "Starting sub process..."
    Invoke-Command -ScriptBlock $SubProcess -ArgumentList $TempDirPath

    Write-Output "Removing temp file/directories..."
    Remove-Item -Path $TempFilePath -Force -Recurse
    Remove-Item -Path $TempDirPath -Force -Recurse

    Write-Output "Successfully setup $ProcessName."
}




# =================== TurboLink ===================
$TurboLink_ProcessName = "TurboLink"
$TurboLink_ZipUri = Get-GoogleDriveDownloadLink -Id "14pMiz4U7R4ZUflB2YEqhKie2v8jChYAv"

function SetupTurboLink {
    param (
        $TempDirPath
    )
   
    $DestinationPath = [System.IO.Path]::Combine($RootDir, "StereoMix/Plugins/TurboLink/Source/ThirdParty")

    Write-Output "Copying ThirdParty libraries..."
    Copy-Item -Path $TempDirPath/* -Destination $DestinationPath -Force -Recurse
}

Setup -ProcessName $TurboLink_ProcessName -ZipUri $TurboLink_ZipUri -SubProcess ${function:SetupTurboLink}


# =================== FMOD SDK ===================
$FMODWin_ProcessName = "FMOD_Win"
$FMODLinux_ProcessName = "FMOD_Linux"
$FMODWin_ZipUri = Get-GoogleDriveDownloadLink -Id "1tR0zvHOeTlOB1XLHPrcrnsVJbstL9Jzl"
$FMODLinux_ZipUri = Get-GoogleDriveDownloadLink -Id "13UgOru-tJnbB1PvcHM2sbRAHoTSq1L8B"

function SetupFMOD {
    param (
        $TempDirPath
    )

    $DestinationPath = [System.IO.Path]::Combine($RootDir, "StereoMix/Plugins")


    Write-Output "Copying Plugins..."
    Copy-Item -Path $TempDirPath/FMODStudio -Destination $DestinationPath -Force -Recurse
    Copy-Item -Path $TempDirPath/FMODStudioNiagara -Destination $DestinationPath -Force -Recurse
}

Setup -ProcessName $FMODWin_ProcessName -ZipUri $FMODWin_ZipUri -SubProcess ${Function:SetupFMOD}
Setup -ProcessName $FMODLinux_ProcessName -ZipUri $FMODLinux_ZipUri -SubProcess ${Function:SetupFMOD}


# =================== Setup Completed ===================
Write-Output "All setup completed."