@echo off

where /q pwsh.exe
IF ERRORLEVEL 0 (
    :: 파워셀 코어로 실행
    pwsh.exe -executionpolicy remotesigned -File ".\setup.ps1" -ShowProgress
) ELSE (
    :: 기본 파워셀로 실행
    powershell.exe -executionpolicy remotesigned -File ".\setup.ps1"
)