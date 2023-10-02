@call :DO_GIT_JOB %1 %~dp0\dotNutBase
@call :DO_GIT_JOB %1 %~dp0\encoding
@call :DO_GIT_JOB %1 %~dp0\marty_cpp
@call :DO_GIT_JOB %1 %~dp0\marty_decimal
@call :DO_GIT_JOB %1 %~dp0\marty_draw_context
@call :DO_GIT_JOB %1 %~dp0\marty_fs_adapters
@call :DO_GIT_JOB %1 %~dp0\marty_simplesquirrel
@call :DO_GIT_JOB %1 %~dp0\marty_vk
@call :DO_GIT_JOB %1 %~dp0\sfmt
@call :DO_GIT_JOB %1 %~dp0\umba


@exit /B


:DO_GIT_JOB
@echo %1'ing %2
@cd %2
@git %1
@cd ..
@echo.
@exit /B

