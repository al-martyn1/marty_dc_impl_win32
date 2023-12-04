@set ROOT_PATH=%1
@set EXE_NAME=%2

@set EXE_SRC_PATH=%ROOT_PATH%\.out\msvc2019\x64\Release
@set NUT_SRC_PATH=%ROOT_PATH%\nuts

@if not exist %EXE_SRC_PATH%\%EXE_NAME%.exe goto EXE_NOT_FOUND
@if not exist %NUT_SRC_PATH%\%EXE_NAME%.nut goto NUT_NOT_FOUND
@goto FILES_FOUND

:EXE_NOT_FOUND
@echo Executable %EXE_NAME%.exe not found in %EXE_SRC_PATH%
@exit /B 1

:NUT_NOT_FOUND
@echo Nut file %EXE_NAME%.nut not found in %NUT_SRC_PATH%
@exit /B 1

:FILES_FOUND

@if not exist .distr mkdir .distr
@if not exist .distr\bin  mkdir .distr\bin
@if not exist .distr\nuts mkdir .distr\nuts

@copy /Y %EXE_SRC_PATH%\%EXE_NAME%.exe .distr\bin\%EXE_NAME%.exe
@copy /Y %NUT_SRC_PATH%\%EXE_NAME%.nut .distr\nuts\%EXE_NAME%.nut

