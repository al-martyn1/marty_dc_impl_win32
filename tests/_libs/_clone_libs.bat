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
git clone %PREFIX%/marty_cpp.git               %~dp0\marty_cpp
git clone %PREFIX%/marty_decimal.git           %~dp0\marty_decimal
git clone %PREFIX%/marty_draw_context.git      %~dp0\marty_draw_context
git clone %PREFIX%/marty_fs_adapters.git       %~dp0\marty_fs_adapters
git clone %PREFIX%/umba_mm_mod_sfmt.git        %~dp0\sfmt
git clone %PREFIX%/umba_mm_mod_umba.git        %~dp0\umba

git clone --recurse-submodules %PREFIX%/simplesquirrel.git          %~dp0\simplesquirrel