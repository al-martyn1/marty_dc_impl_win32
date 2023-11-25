@echo off 
@set TEST_EXE=..\.out\msvc2019\x64\Debug\test_svg_parse.exe
setlocal enableDelayedExpansion 

@echo. >%~dpn0.log

@echo off 
for /R ..\rc\svg %%x in (*.svg) do (
    @echo %%x>>%~dpn0.log 2>&1
    %TEST_EXE% %%x>>%~dpn0.log 2>&1
)

