@set ACC=al-martyn1
@set BASE=github.com


@if "%1"=="SSH"  goto SETUP_SSH
@if "%1"=="GIT"  goto SETUP_SSH
@if "%1"=="HTTP" goto SETUP_HTTP



@rem Default is HTTPS mode

:SETUP_HTTPS
@rem По https 
set PREFIX=https://%BASE%/%ACC%
goto DO_CLONE

:SETUP_HTTP
@rem По https 
set PREFIX=http://%BASE%/%ACC%
goto DO_CLONE

:SETUP_SSH
set PREFIX=git@%BASE%:%ACC%

:DO_CLONE

git clone %PREFIX%/umba_mm_mod_encodings.git   %~dp0\encoding
rem git clone %PREFIX%/marty_adt.git               %~dp0\marty_adt
git clone %PREFIX%/marty_cpp.git               %~dp0\marty_cpp
rem git clone %PREFIX%/marty_csv.git               %~dp0\marty_csv
rem git clone %PREFIX%/marty_dc_impl_win32.git     %~dp0\marty_dc_impl_win32
git clone %PREFIX%/marty_decimal.git           %~dp0\marty_decimal
git clone %PREFIX%/marty_draw_context.git      %~dp0\marty_draw_context
rem git clone %PREFIX%/marty_fs_adapters.git       %~dp0\marty_fs_adapters
rem git clone %PREFIX%/marty_pugixml.git           %~dp0\marty_pugixml
rem git clone %PREFIX%/marty_rcfs.git              %~dp0\marty_rcfs
rem git clone %PREFIX%/marty_rich_text.git         %~dp0\marty_rich_text
rem git clone %PREFIX%/marty_tr.git                %~dp0\marty_tr
rem git clone %PREFIX%/marty_utf.git               %~dp0\marty_utf
rem git clone %PREFIX%/marty_yaml_toml_json.git    %~dp0\marty_yaml_toml_json
git clone %PREFIX%/umba_mm_mod_sfmt.git        %~dp0\sfmt
git clone %PREFIX%/umba_mm_mod_umba.git        %~dp0\umba

