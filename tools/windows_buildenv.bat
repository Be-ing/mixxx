@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
REM this í is just to force some editors to recognize this file as ANSI, not UTF8.

CALL :REALPATH %~dp0\..
SET MIXXX_ROOT=%RETVAL%

IF NOT DEFINED PLATFORM (
    SET PLATFORM=x64
)

IF NOT DEFINED CONFIGURATION (
    SET CONFIGURATION=release-fastbuild
)

IF NOT DEFINED BUILD_ROOT (
    SET BUILD_ROOT=%MIXXX_ROOT%\build
)

IF NOT DEFINED INSTALL_ROOT (
    SET INSTALL_ROOT=%MIXXX_ROOT%\install
)

IF "%~1"=="" (
    REM In case of manual start by double click no arguments are specified: Default to COMMAND_setup
    CALL :COMMAND_setup
    PAUSE
) ELSE (
    CALL :COMMAND_%1
)

EXIT /B 0

:COMMAND_setup
    set VCPKG_BINARY_SOURCES=clear;nuget,azure-artifacts,read

    SET VCPKG_OVERLAY_PORTS=%MIXXX_ROOT%\vcpkg\overlay\ports

    SET CMAKE_TOOLCHAIN_FILE="%MIXXX_ROOT%\vcpkg\scripts\buildsystems\vcpkg.cmake"
    SET CMAKE_PREFIX_PATH=%MIXXX_ROOT%\build\vcpkg_installed\x64-windows
    SET CMAKE_ARGS_EXTRA=-DCMAKE_TOOLCHAIN_FILE=!CMAKE_TOOLCHAIN_FILE! -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=ON

    IF DEFINED GITHUB_ENV (
        nuget sources add -name azure-artifacts -Source https://pkgs.dev.azure.com/mixxx/vcpkg/_packaging/dependencies/nuget/v3/index.json

        ECHO VCPKG_BINARY_SOURCES=!VCPKG_BINARY_SOURCES!>>!GITHUB_ENV!
        ECHO VCPKG_OVERLAY_PORTS=!VCPKG_OVERLAY_PORTS!>>!GITHUB_ENV!
        ECHO CMAKE_PREFIX_PATH=!CMAKE_PREFIX_PATH!>>!GITHUB_ENV!
        ECHO CMAKE_ARGS_EXTRA=!CMAKE_ARGS_EXTRA!>>!GITHUB_ENV!
        ECHO PATH=!PATH!>>!GITHUB_ENV!
    ) else (
        CALL :GENERATE_CMakeSettings_JSON
        echo WARNING: CMakeSettings.json will include an invalid CMAKE_PREFIX_PATH
        echo          for settings other than %CONFIGURATION% .

        IF NOT EXIST %MIXXX_ROOT%\windows_environment_variables.txt (
            nuget sources add -name azure-artifacts -Source https://pkgs.dev.azure.com/mixxx/vcpkg/_packaging/dependencies/nuget/v3/index.json

            ECHO VCPKG_BINARY_SOURCES=!VCPKG_BINARY_SOURCES!>>%MIXXX_ROOT%\windows_environment_variables.txt
            ECHO VCPKG_OVERLAY_PORTS=!VCPKG_OVERLAY_PORTS!>>%MIXXX_ROOT%\windows_environment_variables.txt
            ECHO CMAKE_PREFIX_PATH=!CMAKE_PREFIX_PATH!>>%MIXXX_ROOT%\windows_environment_variables.txt
            ECHO CMAKE_ARGS_EXTRA=!CMAKE_ARGS_EXTRA!>>%MIXXX_ROOT%\windows_environment_variables.txt
        )

        IF NOT EXIST %BUILD_ROOT% (
            ECHO ### Create subdirectory build ###
            MD %BUILD_ROOT%
        )

        IF NOT EXIST %INSTALL_ROOT% (
            ECHO ### Create subdirectory install ###
            MD %INSTALL_ROOT%
        )
    )
    GOTO :EOF

:REALPATH
    SET RETVAL=%~f1
    GOTO :EOF

:GENERATE_CMakeSettings_JSON
REM Generate CMakeSettings.json which is read by MS Visual Studio to determine the supported CMake build environments
    SET CMakeSettings=%MIXXX_ROOT%\CMakeSettings.json
    IF EXIST %CMakeSettings% (
        ECHO ### CMakeSettings.json exist: Rename old file to CMakeSettings__YYYY-MM-DD_HH-MM-SS.json ###
        FOR /f "delims=" %%a in ('wmic OS Get localdatetime ^| find "."') do set DateTime=%%a
        REN %CMakeSettings% CMakeSettings__!DateTime:~0,4!-!DateTime:~4,2!-!DateTime:~6,2!_!DateTime:~8,2!-!DateTime:~10,2!-!DateTime:~12,2!.json
    )
    ECHO ### Create new CMakeSettings.json ###

    CALL :SETANSICONSOLE
    SET OLDCODEPAGE=%RETVAL%
    REM set byte order mark (BOM) for the file .
    REM WARNING: Ensure that the script is saved as ANSI, or these characters will not
    REM contain the correct values. Correct values are EF BB BF (&iuml; &raquo; &iquest;) .
    REM The last character is an actual character for the file, the start "{"
    >"%CMakeSettings%" echo ï»¿{
    CALL :SETUTF8CONSOLE

    >>%CMakeSettings% echo   "configurations": [
    SET configElementTermination=,
    CALL :Configuration2CMakeSettings_JSON off       Debug
    CALL :Configuration2CMakeSettings_JSON legacy    RelWithDebInfo
    CALL :Configuration2CMakeSettings_JSON portable  RelWithDebInfo
    CALL :Configuration2CMakeSettings_JSON fastbuild RelWithDebInfo
    SET configElementTermination=
    CALL :Configuration2CMakeSettings_JSON native    Release
    >>%CMakeSettings% echo   ]
    >>%CMakeSettings% echo }
    CALL :RESTORECONSOLE %OLDCODEPAGE%
    GOTO :EOF

:Configuration2CMakeSettings_JSON <optimize> <configurationtype>
    >>%CMakeSettings% echo     {
    >>%CMakeSettings% echo       "name": "!PLATFORM!__%1",
    >>%CMakeSettings% echo       "buildRoot": "!BUILD_ROOT:\=\\!\\${name}",
    >>%CMakeSettings% echo       "configurationType": "%2",
    >>%CMakeSettings% echo       "enableClangTidyCodeAnalysis": true,
    >>%CMakeSettings% echo       "generator": "Ninja",
    >>%CMakeSettings% echo       "inheritEnvironments": [ "msvc_!PLATFORM!_!PLATFORM!" ],
    >>%CMakeSettings% echo       "installRoot": "!INSTALL_ROOT:\=\\!\\${name}",
    >>%CMakeSettings% echo       "intelliSenseMode": "windows-msvc-!PLATFORM!",
    >>%CMakeSettings% echo       "variables": [
    SET variableElementTermination=,
    CALL :AddCMakeVar2CMakeSettings_JSON "BATTERY"                            "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "BROADCAST"                          "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "BULK"                               "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "CMAKE_EXPORT_COMPILE_COMMANDS"      "BOOL"   "True"
    REM Replace all \ by \\ in CMAKE_PREFIX_PATH
    CALL :AddCMakeVar2CMakeSettings_JSON "CMAKE_PREFIX_PATH"                  "STRING"   "!CMAKE_PREFIX_PATH:\=\\!"
    CALL :AddCMakeVar2CMakeSettings_JSON "DEBUG_ASSERTIONS_FATAL"             "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "HID"                                "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "HSS1394"                            "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "KEYFINDER"                          "BOOL"   "False"
    CALL :AddCMakeVar2CMakeSettings_JSON "LOCALECOMPARE"                      "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "LILV"                               "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "MAD"                                "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "MEDIAFOUNDATION"                    "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "OPUS"                               "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "OPTIMIZE"                           "STRING" "%1"
    CALL :AddCMakeVar2CMakeSettings_JSON "QTKEYCHAIN"                         "BOOL"   "True"
    CALL :AddCMakeVar2CMakeSettings_JSON "STATIC_DEPS"                        "BOOL"   "False"
    SET variableElementTermination=
    CALL :AddCMakeVar2CMakeSettings_JSON "VINYLCONTROL"                       "BOOL"   "True"
    >>%CMakeSettings% echo       ]
    >>%CMakeSettings% echo     }!configElementTermination!
  GOTO :EOF

:AddCMakeVar2CMakeSettings_JSON <varname> <vartype> <value>
    >>%CMakeSettings% echo         {
    >>%CMakeSettings% echo           "name": %1,
    >>%CMakeSettings% echo           "value": %3,
    >>%CMakeSettings% echo           "type": %2
    >>%CMakeSettings% echo         }!variableElementTermination!
  GOTO :EOF

:SETANSICONSOLE
    for /f "tokens=2 delims=:" %%I in ('chcp') do set "_codepage=%%I"

    >NUL chcp 1252

    SET RETVAL=%_codepage%
  GOTO :EOF

:SETUTF8CONSOLE
    >NUL chcp 65001
  GOTO :EOF

:RESTORECONSOLE <codepage>
    >NUL chcp %1
  GOTO :EOF
