if "%APPVEYOR_BUILD_FOLDER%"=="" (
    echo This is expected to be run from AppVeyor.
    exit /b 1
)

set PYTHON="C:\Python37"
set PATH=%PYTHON%\Scripts;%PATH%

%PYTHON%\python %~dp0\..\ci.py %*
