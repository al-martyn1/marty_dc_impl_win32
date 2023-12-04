@call clr_distr.bat
@copy /Y .\.out\msvc2019\x64\Release\TestDrawContext04_GdiOnly.exe .\.out\msvc2019\x64\Release\TestDrawContext04.exe
@call make_distr_files_impl.bat . TestDrawContext04
@if exist ..\..\belka_chivy_nut call make_distr_files_impl.bat ..\..\belka_chivy_nut Belka.Chivy.demo
@call zip_distr.bat squirrel_demos
