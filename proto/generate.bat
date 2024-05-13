@echo off

pushd ..\StereoMix\Plugins\TurboLink
set TURBOLINK_DIR=%CD%
popd

set TOOL_PATH=%TURBOLINK_DIR%\Tools\generate_code.cmd
set OUTPUT_DIR=%TURBOLINK_DIR%\Source\TurboLinkGrpc

for %%i in (*.proto) do (%TOOL_PATH% %%i %OUTPUT_DIR%)