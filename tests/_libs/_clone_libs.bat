@set ACC=al-martyn1
@set BASE=github.com


@if "%1"=="SSH"  goto SETUP_SSH
@if "%1"=="GIT"  goto SETUP_SSH
@if "%1"=="HTTP" goto SETUP_HTTP



@rem Default is HTTPS mode

:SETUP_HTTPS
@rem �� https 
set PREFIX=https://%BASE%/%ACC%
goto DO_CLONE

:SETUP_HTTP
@rem �� https 
set PREFIX=http://%BASE%/%ACC%
goto DO_CLONE

:SETUP_SSH
set PREFIX=git@%BASE%:%ACC%

:DO_CLONE


git clone %PREFIX%/dotNutBase.git              %~dp0\dotNutBase
git clone %PREFIX%/umba_mm_mod_encodings.git   %~dp0\encoding
git clone %PREFIX%/marty_assets_loader.git     %~dp0\marty_assets_loader
git clone %PREFIX%/marty_cpp.git               %~dp0\marty_cpp
git clone %PREFIX%/marty_decimal.git           %~dp0\marty_decimal
git clone %PREFIX%/marty_draw_context.git      %~dp0\marty_draw_context
git clone %PREFIX%/marty_fs_adapters.git       %~dp0\marty_fs_adapters
git clone %PREFIX%/marty_simplesquirrel.git    %~dp0\marty_simplesquirrel
git clone %PREFIX%/marty_virtual_fs.git        %~dp0\marty_virtual_fs
git clone %PREFIX%/marty_vk.git                %~dp0\marty_vk
git clone %PREFIX%/umba_mm_mod_sfmt.git        %~dp0\sfmt
git clone %PREFIX%/umba_mm_mod_umba.git        %~dp0\umba

git clone --recurse-submodules %PREFIX%/simplesquirrel.git          %~dp0\simplesquirrel