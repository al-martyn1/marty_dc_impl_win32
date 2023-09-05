rem @call :DO_GIT_JOB %1 %~dp0\2c_common
@call :DO_GIT_JOB %1 %~dp0\encoding
rem @call :DO_GIT_JOB %1 %~dp0\marty_adt
@call :DO_GIT_JOB %1 %~dp0\marty_cpp
rem @call :DO_GIT_JOB %1 %~dp0\marty_csv
rem @call :DO_GIT_JOB %1 %~dp0\marty_dc_impl_win32
@call :DO_GIT_JOB %1 %~dp0\marty_decimal
@call :DO_GIT_JOB %1 %~dp0\marty_draw_context
rem @call :DO_GIT_JOB %1 %~dp0\marty_fs_adapters
rem @call :DO_GIT_JOB %1 %~dp0\marty_pugixml
rem @call :DO_GIT_JOB %1 %~dp0\marty_rcfs
rem @call :DO_GIT_JOB %1 %~dp0\marty_rich_text
rem @call :DO_GIT_JOB %1 %~dp0\marty_tr
rem @call :DO_GIT_JOB %1 %~dp0\marty_utf
rem @call :DO_GIT_JOB %1 %~dp0\marty_yaml_toml_json
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

